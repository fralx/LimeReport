#ifndef LINESCHART_H
#define LINESCHART_H

#include "lrchartitem.h"

namespace LimeReport {
class LinesChart: public AbstractBarChart{
public:
    LinesChart(ChartItem* chartItem):AbstractBarChart(chartItem){}
    void paintChart(QPainter *painter, QRectF chartRect);
protected:
    void drawDesignMode(QPainter *painter, qreal hStep, qreal vStep, qreal topShift, QRectF barsRect);
    qreal calculateValueYPos(qreal min, qreal max, qreal value, qreal delta, qreal height);
    void paintSeries(QPainter *painter, SeriesItem *series, QRectF barsRect);

private:
    void paintSerialLines(QPainter *painter, QRectF barsRect);
};
}

#endif // LINESCHART_H
