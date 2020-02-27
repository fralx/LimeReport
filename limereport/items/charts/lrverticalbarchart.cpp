#include "lrverticalbarchart.h"

namespace LimeReport{

void VerticalBarChart::paintChart(QPainter *painter, QRectF chartRect)
{
    QRectF calcRect = horizontalLabelsRect(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin(painter),
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
            vPadding(chartRect) + valuesVMargin(painter),
            -hPadding(chartRect),
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintVerticalBars(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin(painter),
            vPadding(chartRect) + valuesVMargin(painter),
            -hPadding(chartRect) * 2,
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintSerialLines(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin(painter),
            vPadding(chartRect) + valuesVMargin(painter),
            -hPadding(chartRect) * 2,
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintHorizontalLabels(painter, calcRect);
}

void VerticalBarChart::paintVerticalBars(QPainter *painter, QRectF barsRect)
{

    int delta = int(maxValue() - minValue());
    delta = genNextValue(delta);

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
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    int delta = int(maxValue() - minValue());
    delta = genNextValue(delta);

    qreal vStep = barsRect.height() / delta;
    qreal hStep = (barsRect.width() / valuesCount());
    qreal topShift = (delta - (maxValue()-minValue())) * vStep + barsRect.top();

    if (!m_chartItem->series().isEmpty()){
        foreach (SeriesItem* series, m_chartItem->series()) {
            if (series->preferredType() == SeriesItem::Line){
                for (int i = 0; i < series->data()->values().count()-1; ++i ){
                    QPoint startPoint = QPoint((i+1)*hStep + barsRect.left()-hStep/2,
                                               (maxValue() * vStep+topShift) - series->data()->values().at(i) * vStep
                                               );
                    QPoint endPoint = QPoint((i+2)*hStep + barsRect.left()-hStep/2,
                                             (maxValue() * vStep+topShift) - series->data()->values().at(i+1) * vStep
                                             );
                    drawSegment(painter, startPoint, endPoint, series->color());
                }
            }
        }
    }
    painter->restore();
}

} // namespace LimeReport
