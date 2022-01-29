#include "lrgridlineschart.h"

namespace LimeReport {
void GridLinesChart::paintChart(QPainter *painter, QRectF chartRect)
{
    updateMinAndMaxValues();

    const qreal valuesHMargin = this->valuesHMargin(painter);
    const qreal valuesVMargin = this->valuesVMargin(painter);

    const qreal hPadding = this->hPadding(chartRect);
    const qreal vPadding = this->vPadding(chartRect);

    QRectF calcRect = horizontalLabelsRect(
        painter,
        chartRect.adjusted(
            hPadding * 2 + valuesHMargin,
            chartRect.height() - (painter->fontMetrics().height() + vPadding*2),
            -(hPadding * 2),
            -vPadding
            )
        );
    const qreal barsShift = calcRect.height();
    const qreal topOffset = painter->fontMetrics().height();
    QRectF gridRect = chartRect.adjusted(
        hPadding,
        vPadding + valuesVMargin + topOffset,
        -hPadding * 3,
        -(vPadding + barsShift)
        );

    if (!m_chartItem->horizontalAxisOnTop()) {
        // Draw labels above the grid
        const qreal height = calcRect.height();
        calcRect.setBottom(gridRect.top());
        calcRect.setTop(calcRect.bottom() - height);
    }

    paintGrid(painter, gridRect);

    paintSerialLines(
        painter,
        gridRect.adjusted(hPadding + valuesHMargin, 0, 0, 0)
        );
    paintHorizontalLabels(painter, calcRect);
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
