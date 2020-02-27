#ifndef LINESCHART_H
#define LINESCHART_H

#include "lrchartitem.h"

namespace LimeReport {
class LinesChart: public AbstractBarChart{
public:
    LinesChart(ChartItem* chartItem):AbstractBarChart(chartItem){}
    void paintChart(QPainter *painter, QRectF chartRect);
private:
    void paintSerialLines(QPainter *painter, QRectF barsRect);
    void drawDesignMode(QPainter *painter, qreal hStep, qreal vStep, qreal topShift, QRectF barsRect);
};
}

#endif // LINESCHART_H
