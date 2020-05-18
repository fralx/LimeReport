#include "lrlineschart.h"

namespace LimeReport {

void LinesChart::paintChart(QPainter *painter, QRectF chartRect)
{
    QRectF calcRect = horizontalLabelsRect(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin(painter),
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
            vPadding(chartRect) + valuesVMargin(painter),
            -hPadding(chartRect),
            -(vPadding(chartRect) + barsShift)
        )
    );
    paintSerialLines(
        painter,
        chartRect.adjusted(
            hPadding(chartRect) * 2 + valuesHMargin(painter),
            vPadding(chartRect) + valuesVMargin(painter),
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

void LinesChart::paintSerialLines(QPainter* painter, QRectF barsRect)
{
    if (valuesCount() == 0) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    int delta = int(maxValue() - minValue());
    delta = genNextValue(delta);

    qreal vStep = barsRect.height() / delta;
    qreal hStep = barsRect.width() / valuesCount();
    qreal topShift = (delta - (maxValue() - minValue())) * vStep +barsRect.top();

    if (m_chartItem->itemMode() != DesignMode){
        foreach (SeriesItem* series, m_chartItem->series()) {
            QPen pen(series->color());
            pen.setWidth(4);
            painter->setPen(pen);
            for (int i = 0; i < series->data()->values().count()-1; ++i ){
                QPoint startPoint = QPoint((i+1) * hStep + barsRect.left() - hStep/2,
                                           (maxValue()*vStep+topShift) - series->data()->values().at(i) * vStep);
                QPoint endPoint = QPoint((i+2) * hStep + barsRect.left() - hStep/2,
                                         (maxValue() * vStep+topShift) - series->data()->values().at(i+1) * vStep);
                drawSegment(painter, startPoint, endPoint, series->color());
            }
        }
    } else {
        drawDesignMode(painter, hStep, vStep, topShift, barsRect);
    }
    painter->restore();
}

} //namespace LimeReport

