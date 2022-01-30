#include "lrgridlineschart.h"

namespace LimeReport {
void GridLinesChart::paintChart(QPainter *painter, QRectF chartRect)
{
    updateMinAndMaxValues();

    const qreal hPadding = this->hPadding(chartRect);
    const qreal vPadding = this->vPadding(chartRect);

    const qreal valuesVMargin = this->valuesVMargin(painter);

    QRectF gridRect = chartRect.adjusted(
        hPadding,
        vPadding + valuesVMargin * 2,
        -hPadding * 3,
        -vPadding * 3
        );

    if (!m_chartItem->horizontalAxisOnTop()) {
        // If horizontal axis is on the bottom, move grid a little up
        gridRect.adjust(0, -valuesVMargin, 0 , -valuesVMargin);
    }

    // Adapt font for horizontal axis
    painter->setFont(adaptFont((gridRect.width() - this->valuesHMargin(painter)) / xAxisData().segmentCount() * 0.8,
                               painter->font(),
                               xAxisData()));

    const qreal valuesHMargin = this->valuesHMargin(painter);

    // Adjust vertical axis labels padding
    gridRect.adjust(valuesHMargin * 0.2, 0, 0, 0);

    paintGrid(painter, gridRect);

    paintSerialLines(
        painter,
        gridRect.adjusted(hPadding + valuesHMargin, 0, 0, 0)
        );
}

void GridLinesChart::paintSerialLines(QPainter* painter, QRectF barsRect)
{
    if (valuesCount() == 0) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    const AxisData &yAxisData = this->yAxisData();
    const qreal delta = yAxisData.delta();

    if (m_chartItem->itemMode() == DesignMode){
        const qreal hStep = barsRect.width() / valuesCount();
        const qreal vStep = barsRect.height() / delta;
        const qreal topShift = (delta - (maxValue() - minValue())) * vStep + barsRect.top();
        drawDesignMode(painter, hStep, vStep, topShift, barsRect);
        painter->restore();
        return;
    }

    const AxisData &xAxisData = this->xAxisData();
    const qreal hStep = barsRect.width() / (xAxisData.rangeMax() - xAxisData.rangeMin());

    qreal leftMargin = 0;
    const qreal topMargin = barsRect.top();

    for (SeriesItem* series : m_chartItem->series()) {
        QPen pen(series->color());
        pen.setWidth(m_chartItem->seriesLineWidth());
        painter->setPen(pen);

        const QList<qreal> &xAxisValues = series->data()->xAxisValues();
        const QList<qreal> &values = series->data()->values();
        const int xAxisValuesSize = xAxisValues.size();
        qreal lastXPos = 0;
        qreal lastYPos = 0;
        if (!values.isEmpty()) {
            // Calculate first point position on plot before loop
            lastYPos = calculatePos(yAxisData, values.first(), barsRect.height());
        }
        if (xAxisValues.isEmpty()) {
            leftMargin = barsRect.left();
        } else {
            leftMargin = barsRect.left();
            lastXPos = calculatePos(xAxisData, xAxisValues.first(), barsRect.width());
        }
        for (int i = 0; i < values.count() - 1; ++i ) {
            const qreal startY = lastYPos;
            const qreal endY = calculatePos(yAxisData, values.at(i+1), barsRect.height());
            // Record last used Y position to only calculate new one
            lastYPos = endY;

            qreal startX = lastXPos;
            qreal endX = 0;
            if (i + 1 < xAxisValuesSize) {
                endX = calculatePos(xAxisData, xAxisValues.at(i+1), barsRect.width());
            } else {
                endX = startX + hStep;
            }
            // Record last used X position to only calculate new one
            lastXPos = endX;

            QPoint startPoint = QPoint(startX + leftMargin, startY + topMargin);
            QPoint endPoint = QPoint(endX + leftMargin, endY + topMargin);
            drawSegment(painter, startPoint, endPoint, series->color());
        }
    }

    painter->restore();
}
}
