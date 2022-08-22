#include "lrverticalbarchart.h"

namespace LimeReport{

void VerticalBarChart::paintChart(QPainter *painter, QRectF chartRect)
{
    updateMinAndMaxValues();

    const qreal valuesHMargin = this->valuesHMargin(painter);
    const qreal valuesVMargin = this->valuesVMargin(painter);

    QRectF calcRect = horizontalLabelsRect(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin,
            chartRect.height() - (painter->fontMetrics().height() + vPadding(chartRect) * 2),
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
    paintVerticalBars(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin,
            vPadding(chartRect) + valuesVMargin,
            -hPadding(chartRect) * 2,
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintSerialLines(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin,
            vPadding(chartRect) + valuesVMargin,
            -hPadding(chartRect) * 2,
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintHorizontalLabels(painter, calcRect);
}

void VerticalBarChart::paintVerticalBars(QPainter *painter, QRectF barsRect)
{

    if (valuesCount() == 0) return;

    const AxisData &yAxisData = this->yAxisData();
    const qreal delta = yAxisData.delta();

    int barSeriesCount = 0;
    foreach(SeriesItem* series, m_chartItem->series()){
        if (series->preferredType() == SeriesItem::Bar) barSeriesCount++;
    }

    barSeriesCount = (m_chartItem->itemMode() == DesignMode) ? seriesCount() : barSeriesCount;
    if (barSeriesCount < 1) return;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,false);

    qreal vStep = barsRect.height() / delta;
    qreal hStep = (barsRect.width() / valuesCount()) / (barSeriesCount == 0 ? 1 : barSeriesCount);
    qreal topShift = (delta - (maxValue() - minValue())) * vStep + barsRect.top();

    if (!m_chartItem->series().isEmpty() && (m_chartItem->itemMode() != DesignMode)){
        int curSeries = 0;
        foreach (SeriesItem* series, m_chartItem->series()) {
            if (series->preferredType() == SeriesItem::Bar){
                qreal curHOffset = curSeries * hStep + barsRect.left();
                painter->setBrush(series->color());
                foreach (qreal value, series->data()->values()) {
                    painter->drawRect(QRectF(curHOffset, maxValue() * vStep + topShift, hStep, -value * vStep));
                    curHOffset += hStep * barSeriesCount;
                }
                curSeries++;
            }
        }
    } else {
        qreal curHOffset = barsRect.left();
        int curColor = 0;
        for (int i = 0; i < 9; ++i){
            if (curColor == 3) curColor = 0;
            painter->setBrush(color_map[curColor]);
            painter->drawRect(QRectF(curHOffset, maxValue() * vStep + barsRect.top(), hStep, -designValues()[i] * vStep));
            curHOffset += hStep;
            curColor++;
        }
    }
    painter->restore();
}

void VerticalBarChart::paintSerialLines(QPainter* painter, QRectF barsRect)
{
    if (valuesCount() == 0 || m_chartItem->series().isEmpty() ) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    for (SeriesItem *series : m_chartItem->series()) {
        if (series->preferredType() == SeriesItem::Line){
            paintSeries(painter, series, barsRect);
        }
    }
    painter->restore();
}

} // namespace LimeReport
