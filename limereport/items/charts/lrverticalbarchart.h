#ifndef VERTICALBARCHART_H
#define VERTICALBARCHART_H

#include "lrlineschart.h"

namespace LimeReport{

class VerticalBarChart: public LinesChart{
public:
    VerticalBarChart(ChartItem* chartItem):LinesChart(chartItem){}
    void paintChart(QPainter *painter, QRectF chartRect);
//    void paintVerticalGrid(QPainter *painter, QRectF gridRect);
    void paintVerticalBars(QPainter *painter, QRectF barsRect);
    void paintSerialLines(QPainter *painter, QRectF barsRect);
};

} //namespace LimeReport

#endif // VERTICALBARCHART_H
