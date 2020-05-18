#include "lrhorizontalbarchart.h"

namespace LimeReport{

void HorizontalBarChart::paintChart(QPainter *painter, QRectF chartRect)
{
    QRectF calcRect = verticalLabelsRect(painter, chartRect.adjusted(
                                              hPadding(chartRect),
                                              vPadding(chartRect) * 2,
                                              -(chartRect.width() * 0.9),
                                              -(vPadding(chartRect) * 2 + valuesVMargin(painter))
                                              ));

    qreal barsShift = calcRect.width();

    paintHorizontalGrid(painter, chartRect.adjusted(
                                     hPadding(chartRect) + barsShift,
                                     vPadding(chartRect),
                                     -(hPadding(chartRect)),-vPadding(chartRect)));
    paintHorizontalBars(painter, chartRect.adjusted(
                                     hPadding(chartRect) + barsShift,
                                     vPadding(chartRect) * 2,
                                     -(hPadding(chartRect)),
                                     -(vPadding(chartRect) * 2) ));

    paintVerticalLabels(painter, calcRect);
}

void HorizontalBarChart::paintHorizontalBars(QPainter *painter, QRectF barsRect)
{
    if (seriesCount() == 0) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,false);
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    qreal vStep = (barsRect.height()-painter->fontMetrics().height()) / valuesCount() / seriesCount();
    qreal hStep = (barsRect.width()-painter->fontMetrics().width(QString::number(maxValue()))) / delta;

    if (!m_chartItem->series().isEmpty() && (m_chartItem->itemMode() != DesignMode)){
        int curSeries = 0;
        foreach (SeriesItem* series, m_chartItem->series()) {
            qreal curVOffset = curSeries*vStep+barsRect.top();
            painter->setBrush(series->color());
            foreach (qreal value, series->data()->values()) {
                painter->drawRect(QRectF((-minValue()*hStep)+barsRect.left(), curVOffset, value*hStep, vStep));
                curVOffset+=vStep*seriesCount();
            }
            curSeries++;
        }
    } else {
        qreal curVOffset = barsRect.top();
        int curColor = 0;
        for (int i=0; i<9; ++i){
            if (curColor==3) curColor=0;
            painter->setBrush(color_map[curColor]);
            painter->drawRect(QRectF(barsRect.left(), curVOffset, designValues()[i]*hStep, vStep));
            curVOffset+=vStep;
            curColor++;
        }
    }
    painter->restore();
}

}
