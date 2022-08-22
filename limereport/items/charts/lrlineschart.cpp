#include "lrlineschart.h"

namespace LimeReport {

void LinesChart::paintChart(QPainter *painter, QRectF chartRect)
{
    updateMinAndMaxValues();

    const qreal valuesHMargin = this->valuesHMargin(painter);
    const qreal valuesVMargin = this->valuesVMargin(painter);

    QRectF calcRect = horizontalLabelsRect(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin,
            chartRect.height() - (painter->fontMetrics().height() + vPadding(chartRect)*2),
            -(hPadding(chartRect) * 2),
            -vPadding(chartRect)
        )
    );
    qreal barsShift = calcRect.height();
    paintVerticalGrid(
        painter,
        chartRect.adjusted(
            hPadding(chartRect),
            vPadding(chartRect) + valuesVMargin,
            -hPadding(chartRect),
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintSerialLines(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin,
            vPadding(chartRect) + valuesVMargin,
            -(hPadding(chartRect) * 2),
            -(vPadding(chartRect)+barsShift)
        )
    );
    paintHorizontalLabels(painter, calcRect);
}

void LinesChart::drawDesignMode(QPainter* painter, qreal hStep, qreal vStep, qreal topShift, QRectF barsRect){
    for (int i = 0; i < valuesCount()-1; ++i){
        QPoint startPoint = QPoint((i+1) * hStep + barsRect.left() - hStep/2,
                                   (maxValue() * vStep+topShift) - designValues()[i] * vStep
                                   );
        QPoint endPoint = QPoint((i+2) * hStep + barsRect.left() - hStep/2,
                                 (maxValue() * vStep+topShift) - designValues()[i+1] * vStep
                                 );
        drawSegment(painter, startPoint, endPoint, color_map[0]);

        startPoint = QPoint((i+1) * hStep + barsRect.left() - hStep/2,
                            (maxValue() * vStep+topShift) - designValues()[i+3] * vStep
                            );
        endPoint = QPoint((i+2) * hStep + barsRect.left() - hStep/2,
                          (maxValue() * vStep+topShift) - designValues()[i+3+1] * vStep
                          );
        drawSegment(painter, startPoint, endPoint, color_map[1]);

        startPoint = QPoint((i+1) * hStep + barsRect.left() - hStep/2,
                            (maxValue() * vStep+topShift) - designValues()[i+6] * vStep
                            );
        endPoint = QPoint((i+2) * hStep + barsRect.left() - hStep/2,
                          (maxValue() * vStep+topShift) - designValues()[i+6+1] * vStep
                          );
        drawSegment(painter, startPoint, endPoint, color_map[2]);
    }
}

qreal LinesChart::calculatePos(const AxisData &data, qreal value, qreal rectSize) const
{
    if (data.type() == AxisData::XAxis || (data.reverseDirection() && data.rangeMin() >= 0)) {
        // Not flipping for minimum less than 0 because lower number is at the bottom.
        return (1 - (data.rangeMax() - value) / data.delta()) * rectSize;
    } else {
        return (data.rangeMax() - value) / data.delta() * rectSize;
    }
}

void LinesChart::paintSeries(QPainter *painter, SeriesItem *series, QRectF barsRect)
{
    const AxisData &yAxisData = this->yAxisData();
    const AxisData &xAxisData = this->xAxisData();

    const qreal xAxisDiff = std::max(1.0, xAxisData.maxValue() - xAxisData.minValue());
    const qreal hStep = barsRect.width() / xAxisDiff;
    const qreal topMargin = barsRect.top();

    QPen pen(series->color());
    pen.setWidth(4);
    painter->setPen(pen);

    const QList<qreal> &values = series->data()->values();

    qreal lastYValue = 0;
    qreal lastXValue = barsRect.left() + hStep/2;
    if (!values.isEmpty()) {
        // Calculate first point position on plot before loop
        lastYValue = calculatePos(yAxisData, values.first(), barsRect.height());
    }
    for (int i = 0; i < values.count()-1; ++i ){
        const qreal startY = lastYValue;
        const qreal endY = calculatePos(yAxisData, values.at(i+1), barsRect.height());
        // Record last used Y position to only calculate new one
        lastYValue = endY;

        const qreal startX = lastXValue;
        const qreal endX = startX + hStep;
        // Record last used X position to only calculate new one
        lastXValue = endX;

        QPoint startPoint = QPoint(startX, startY + topMargin);
        QPoint endPoint = QPoint(endX, endY + topMargin);
        drawSegment(painter, startPoint, endPoint, series->color());
    }
}

void LinesChart::paintSerialLines(QPainter* painter, QRectF barsRect)
{
    if (valuesCount() == 0) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    if (m_chartItem->itemMode() == DesignMode){
        const AxisData &yAxisData = this->yAxisData();
        const qreal delta = yAxisData.delta();
        const qreal hStep = barsRect.width() / valuesCount();
        const qreal vStep = barsRect.height() / delta;
        const qreal topShift = (delta - (maxValue() - minValue())) * vStep + barsRect.top();
        drawDesignMode(painter, hStep, vStep, topShift, barsRect);
        painter->restore();
        return;
    }

    for (SeriesItem *series : m_chartItem->series()) {
        paintSeries(painter, series, barsRect);
    }

    painter->restore();
}

} //namespace LimeReport

