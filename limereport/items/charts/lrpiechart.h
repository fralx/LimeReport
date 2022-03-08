#ifndef PIECHART_H
#define PIECHART_H

#include "lrchartitem.h"

namespace LimeReport{

class PieChart : public AbstractChart{
public:
    PieChart(ChartItem* chartItem):AbstractChart(chartItem){}
    QSizeF calcChartLegendSize(const QFont &font, qreal maxWidth = 0);
    void paintChart(QPainter *painter, QRectF chartRect);
    void paintChartLegend(QPainter *painter, QRectF legendRect);
protected:
    void drawPercent(QPainter *painter, QRectF chartRect, qreal startAngle, qreal angle);
};

} // namespace LimeReport
#endif // PIECHART_H
