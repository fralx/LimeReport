#include "lrpiechart.h"

namespace LimeReport{

void PieChart::drawPercent(QPainter *painter, QRectF chartRect, qreal startAngle, qreal angle)
{
    painter->save();

    QPointF center(chartRect.left()+chartRect.width()/2,chartRect.top()+chartRect.height()/2);
    qreal percent = angle/3.6;
#if QT_VERSION < 0x050000
    qreal radAngle = (angle/2+startAngle)*(M_PI/180);
#else
    qreal radAngle = qDegreesToRadians(angle/2+startAngle);
#endif
    qreal radius = painter->fontMetrics().boundingRect("99,9%").width();
    qreal border = chartRect.height()*0.02;
    qreal length = (chartRect.height())/2-(radius/2+border);
    qreal x,y;
    x = length*qCos(radAngle);
    y = length*qSin(radAngle);
    QPointF endPoint(center.x()+x,center.y()-y);
    painter->setPen(Qt::white);
    QRectF textRect(endPoint.x()-(radius/2),endPoint.y()-(radius/2),radius,radius);

    qreal arcLength = 3.14 * length * angle / 180;
    if (arcLength >= radius)
        painter->drawText(textRect,Qt::AlignCenter,QString::number(percent,'f',1)+"%");
    painter->restore();

}

void PieChart::paintChart(QPainter *painter, QRectF chartRect)
{
    painter->save();
    QPen pen(Qt::white);
    pen.setWidthF(2);
    painter->setPen(pen);

    QBrush brush(Qt::transparent);
    painter->setBrush(brush);
    painter->setBackground(QBrush(Qt::NoBrush));

    QRectF tmpRect = chartRect;
    if (chartRect.height()>chartRect.width()){
        tmpRect.setHeight(chartRect.width());
        tmpRect.adjust(0,(chartRect.bottom()-tmpRect.bottom())/2,
                       0,(chartRect.bottom()-tmpRect.bottom())/2);
    } else {
        tmpRect.setWidth(chartRect.height());
    }

    chartRect = tmpRect;
    painter->drawRect(chartRect);

    if (!m_chartItem->series().isEmpty()&&!m_chartItem->series().at(0)->data()->values().isEmpty()){
        SeriesItem* si = m_chartItem->series().at(0);
        qreal sum = 0;
        foreach(qreal value, si->data()->values()){
            sum+=value;
        }
        qreal onePercent = sum / 100;
        qreal currentDegree = 0;
        for(int i=0; i<si->data()->values().count(); ++i){
            qreal value = si->data()->values().at(i);
            qreal sectorDegree = (value/onePercent)*3.6;
            painter->setBrush(si->data()->colors().at(i));
            painter->drawPie(chartRect,currentDegree*16,sectorDegree*16);
            drawPercent(painter, chartRect, currentDegree, sectorDegree);
            currentDegree += sectorDegree;
        }
    } else if (m_chartItem->itemMode() == DesignMode){
        painter->setBrush(color_map[0]);
        painter->drawPie(chartRect,0,260*16);
        drawPercent(painter, chartRect, 0, 260);
        painter->setBrush(color_map[1]);
        painter->drawPie(chartRect,260*16,40*16);
        drawPercent(painter, chartRect, 260, 40);
        painter->setBrush(color_map[2]);
        painter->drawPie(chartRect,300*16,60*16);
        drawPercent(painter, chartRect, 300, 60);
    }

    pen.setWidthF(1);
    pen.setColor(Qt::gray);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(chartRect);
    painter->restore();
}

void PieChart::paintChartLegend(QPainter *painter, QRectF legendRect)
{
    prepareLegendToPaint(legendRect, painter);

    int indicatorSize = painter->fontMetrics().height()/2;
    painter->setRenderHint(QPainter::Antialiasing,false);

    if (m_chartItem->drawLegendBorder())
        painter->drawRect(legendRect);

    painter->setRenderHint(QPainter::Antialiasing,true);
    QRectF indicatorsRect = legendRect.adjusted(painter->fontMetrics().height()/2,painter->fontMetrics().height()/2,0,0);

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
        qreal cw = 0;
        SeriesItem* si = m_chartItem->series().at(0);
        for (int i=0;i<si->data()->labels().count();++i){
            QString label = si->data()->labels().at(i);
            painter->setPen(Qt::black);
            painter->drawText(indicatorsRect.adjusted(indicatorSize+indicatorSize/2,cw,0,0),label);
            painter->setPen(si->data()->colors().at(i));
            painter->setBrush(si->data()->colors().at(i));
            painter->drawEllipse(
                indicatorsRect.adjusted(
                    0,
                    cw+indicatorSize/2,
                    -(indicatorsRect.width()-indicatorSize),
                    -(indicatorsRect.height()-(cw+indicatorSize+indicatorSize/2))
                    )
                );
            cw += painter->fontMetrics().height();
        }
    } else if (m_chartItem->itemMode() == DesignMode){
        qreal cw = 0;
        for (int i=0;i<m_designLabels.size();++i){
            QString label = m_designLabels.at(i);
            painter->setPen(Qt::black);
            painter->drawText(indicatorsRect.adjusted(indicatorSize+indicatorSize/2,cw,0,0),label);
            painter->setBrush(color_map[i]);
            painter->setPen(color_map[i]);
            painter->drawEllipse(
                indicatorsRect.adjusted(
                    0,
                    cw+indicatorSize/2,
                    -(indicatorsRect.width()-indicatorSize),
                    -(indicatorsRect.height()-(cw+indicatorSize+indicatorSize/2))
                    )
                );
            cw += painter->fontMetrics().height();
        }

    }
}

QSizeF PieChart::calcChartLegendSize(const QFont &font, qreal)
{
    QFontMetrics fm(font);

    qreal cw = 0;
    qreal maxWidth = 0;

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
        SeriesItem* si = m_chartItem->series().at(0);
        foreach(QString label, si->data()->labels()){
            cw += fm.height();
            if (maxWidth<fm.boundingRect(label).width())
                maxWidth = fm.boundingRect(label).width()+10;
        }
    } else {
        foreach(QString label, m_designLabels){
            cw += fm.height();
            if (maxWidth<fm.boundingRect(label).width())
                maxWidth = fm.boundingRect(label).width()+10;
        }
    }
    cw += fm.height();
    return  QSizeF(maxWidth+fm.height()*2,cw);
}

}
