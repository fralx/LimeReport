#ifndef HORIZONTALBARCHART_H
#define HORIZONTALBARCHART_H

#include "lrchartitem.h"

namespace LimeReport{

class HorizontalBarChart: public AbstractBarChart{
public:
    HorizontalBarChart(ChartItem* chartItem):AbstractBarChart(chartItem){}
    void paintChart(QPainter *painter, QRectF chartRect);
    void paintHorizontalBars(QPainter *painter, QRectF barsRect);
};

} // namespace LimeReport

#endif // HORIZONTALBARCHART_H
