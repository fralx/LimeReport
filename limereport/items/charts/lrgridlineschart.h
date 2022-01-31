#ifndef GRIDLINESCHART_H
#define GRIDLINESCHART_H

#include "lrlineschart.h"

namespace LimeReport {
class GridLinesChart : public LinesChart{
public:
    GridLinesChart(ChartItem* chartItem):LinesChart(chartItem){}
    void paintChart(QPainter *painter, QRectF chartRect);

private:
    void paintSerialLines(QPainter *painter, QRectF barsRect);
};
}

#endif // GRIDLINESCHART_H
