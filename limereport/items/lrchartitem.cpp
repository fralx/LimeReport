#include "lrchartitem.h"
#include <QStyleOptionGraphicsItem>
#include <QPainter>

#include "lrdesignelementsfactory.h"
#include "lrchartitemeditor.h"
#include "lrdatasourcemanager.h"
#include "lrpagedesignintf.h"
#include "lrreportengine_p.h"
#include "lrdatadesignintf.h"

namespace{

const QString xmlTag = "ChartItem";

LimeReport::BaseDesignIntf * createChartItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::ChartItem(owner,parent);
}
bool registred = LimeReport::DesignElementsFactory::instance().registerCreator(
                     xmlTag, LimeReport::ItemAttribs(QObject::tr("Chart Item"),"Item"), createChartItem
                 );
}

namespace LimeReport{

QColor generateColor()
{
    int red = (qrand()%(256 - 1)) + 1;
    int green = (qrand()%(256 - 1)) + 1;
    int blue = (qrand()%(256 - 1)) + 1;;
    return QColor(red,green,blue);
}

QColor color_map[39] = {
        QColor(51,102,204), QColor(220,57,18), QColor(225, 153, 0), QColor(16, 150, 24), QColor(153,0,153),
        QColor(0,153,198), QColor(221, 68, 119),
        QColor(255,0,0), QColor(0,0,139), QColor(0,205,0), QColor(233,30,99), QColor(255,255,0), QColor(244,67,54),
        QColor(156,39,176),  QColor(103,58,183), QColor(63,81,181), QColor(33,153,243),
        QColor(0,150,136), QColor(78,175,80), QColor(139,195,74), QColor(205,228,57),
        QColor(0,139,0), QColor(0,0,255), QColor(255,235,59), QColor(255,193,7),
        QColor(255,152,0), QColor(255,87,34), QColor(121,85,72), QColor(158,158,158),
        QColor(96,125,139), QColor(241,153,185),  QColor(64,64,64),
        QColor(188,229,218), QColor(139,0,0), QColor(139,139,0), QColor(171, 130, 255),
        QColor(139, 123, 139), QColor(255, 0, 255), QColor(139, 69, 19)
};

QString SeriesItem::name() const
{
    return m_name;
}

void SeriesItem::setName(const QString &name)
{
    m_name = name;
}

QString SeriesItem::valuesColumn() const
{
    return m_valuesColumn;
}

void SeriesItem::setValuesColumn(const QString &valuesColumn)
{
    m_valuesColumn = valuesColumn;
}

QString SeriesItem::labelsColumn() const
{
    return m_labelsColumn;
}

void SeriesItem::setLabelsColumn(const QString &labelsColumn)
{
    m_labelsColumn = labelsColumn;
}

SeriesItem *SeriesItem::clone()
{
    SeriesItem* result = new SeriesItem();
    for (int i = 0; i < this->metaObject()->propertyCount(); ++i){
        result->setProperty(this->metaObject()->property(i).name(),property(this->metaObject()->property(i).name()));
    }
    return result;
}

void SeriesItem::fillSeriesData(IDataSource *dataSource)
{
    if (dataSource){
        dataSource->first();
        int currentColorIndex = 0;
        while(!dataSource->eof()){
            if (!m_labelsColumn.isEmpty())
                m_data.labels().append(dataSource->data(m_labelsColumn).toString());
            m_data.values().append(dataSource->data(m_valuesColumn).toDouble());
            m_data.colors().append((currentColorIndex<32)?color_map[currentColorIndex]:generateColor());
            dataSource->next();
            currentColorIndex++;
        }
    }
}

QColor SeriesItem::color() const
{
    return m_color;
}

void SeriesItem::setColor(const QColor &color)
{
    m_color = color;
}

SeriesItem::SeriesItemPreferredType SeriesItem::preferredType() const
{
    return m_preferredType;
}

void SeriesItem::setPreferredType(const SeriesItemPreferredType& type)
{
    m_preferredType = type;
}

ChartItem::ChartItem(QObject *owner, QGraphicsItem *parent)
    : ItemDesignIntf(xmlTag, owner, parent), m_legendBorder(true),
      m_legendAlign(LegendAlignCenter), m_titleAlign(TitleAlignCenter),
      m_chartType(Pie), m_labelsField("")
{
    m_labels<<"First"<<"Second"<<"Thrid";
    m_chart = new PieChart(this);
}

ChartItem::~ChartItem()
{
    foreach (SeriesItem* series, m_series) {
        delete series;
    }
    m_series.clear();
    delete m_chart;
}

ChartItem::TitleAlign ChartItem::titleAlign() const
{
    return m_titleAlign;
}

void ChartItem::setTitleAlign(const TitleAlign &titleAlign)
{
    if (m_titleAlign != titleAlign){
        TitleAlign oldValue = m_titleAlign;
        m_titleAlign = titleAlign;
        notify("titleAlign",oldValue,m_titleAlign);
        update();
    }
}

void ChartItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    setupPainter(painter);
    painter->setFont(transformToSceneFont(painter->font()));
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setRenderHint(QPainter::TextAntialiasing,true);
    qreal borderMargin = (rect().height()*0.01>10)?(10):(rect().height()*0.01);
    qreal maxTitleHeight = rect().height()*0.2;

    QFont tmpFont = painter->font();

    qreal titleOffset = !m_title.isEmpty()?(((painter->fontMetrics().height()+borderMargin*2)<maxTitleHeight)?
                        (painter->fontMetrics().height()+borderMargin*2):
                        (maxTitleHeight)):0;

    QRectF titleRect = QRectF(borderMargin,borderMargin,rect().width()-borderMargin*2,titleOffset);
    QRectF legendRect = m_chart->calcChartLegendRect(painter->font(), rect(), false, borderMargin, titleOffset);
    QRectF diagramRect = rect().adjusted(borderMargin,titleOffset+borderMargin,
                                         -(legendRect.width()+borderMargin*2),-borderMargin);

    paintChartTitle(painter, titleRect);
    m_chart->paintChartLegend(painter,legendRect);
    m_chart->paintChart(painter,diagramRect);

    painter->restore();
    ItemDesignIntf::paint(painter,option,widget);
}

BaseDesignIntf *ChartItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    ChartItem* result = new ChartItem(owner,parent);
    foreach (SeriesItem* series, m_series) {
        result->m_series.append(series->clone());
    }
    return result;
}

QObject *ChartItem::createElement(const QString &collectionName, const QString &elementType)
{
    Q_UNUSED(elementType);
    if (collectionName.compare("series")==0){
        SeriesItem* seriesItem = new SeriesItem();
        m_series.append(seriesItem);
        return seriesItem;
    }
    return 0;
}

int ChartItem::elementsCount(const QString &collectionName)
{
    if (collectionName.compare("series")==0)
        return m_series.count();
    return 0;
}

QObject *ChartItem::elementAt(const QString &collectionName, int index)
{
    if (collectionName.compare("series")==0)
        return m_series.at(index);
    return 0;
}

void ChartItem::updateItemSize(DataSourceManager *dataManager, RenderPass , int )
{
    if (dataManager && dataManager->dataSource(m_datasource)){
        IDataSource* ds =  dataManager->dataSource(m_datasource);
        foreach (SeriesItem* series, m_series) {
            series->setLabelsColumn(m_labelsField);
            series->fillSeriesData(ds);
        }
        fillLabels(ds);
    }
}

void ChartItem::fillLabels(IDataSource *dataSource)
{
    m_labels.clear();
    if (dataSource && !m_labelsField.isEmpty()){
        dataSource->first();
        while(!dataSource->eof()){
            m_labels.append(dataSource->data(m_labelsField).toString());
            dataSource->next();
        }
    }
}

QWidget *ChartItem::defaultEditor()
{
    QSettings* l_settings = (page()->settings() != 0) ?
                                 page()->settings() :
                                 (page()->reportEditor()!=0) ? page()->reportEditor()->settings() : 0;
    QWidget* editor = new ChartItemEditor(this,page(),l_settings);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    return editor;
}

QList<QString> ChartItem::labels() const
{
    return m_labels;
}

void ChartItem::setLabels(const QList<QString> &labels)
{
    m_labels = labels;
}

QString ChartItem::labelsField() const
{
    return m_labelsField;
}

void ChartItem::setLabelsField(const QString &labelsField)
{
    m_labelsField = labelsField;
}

ChartItem::ChartType ChartItem::chartType() const
{
    return m_chartType;
}

void ChartItem::setChartType(const ChartType &chartType)
{
    if (m_chartType != chartType){
        ChartType oldValue = m_chartType;
        m_chartType = chartType;
        delete m_chart;
        switch (m_chartType) {
        case Pie:
            m_chart = new PieChart(this);
            break;
        case VerticalBar:
            m_chart = new VerticalBarChart(this);
            break;
        case HorizontalBar:
            m_chart = new HorizontalBarChart(this);
            break;
        }
        notify("chartType",oldValue,m_chartType);
        update();
    }
}

QString ChartItem::datasource() const
{
    return m_datasource;
}

void ChartItem::setDatasource(const QString &datasource)
{
    m_datasource = datasource;
}

void ChartItem::paintChartTitle(QPainter *painter, QRectF titleRect)
{
    painter->save();
    QFont tmpFont = painter->font();
    QFontMetrics fm(tmpFont);
    while ((fm.height()>titleRect.height() || fm.width(m_title)>titleRect.width())
           && tmpFont.pixelSize()>1) {
        tmpFont.setPixelSize(tmpFont.pixelSize()-1);
        fm = QFontMetrics(tmpFont);
    }
    painter->setFont(tmpFont);
    Qt::AlignmentFlag align = Qt::AlignCenter;
    switch (m_titleAlign) {
    case TitleAlignLeft:
        align = Qt::AlignLeft;
        break;
    case TitleAlignCenter:
        align = Qt::AlignCenter;
        break;
    case TitleAlignRight:
        align = Qt::AlignRight;
        break;
    }
    painter->drawText(titleRect, align, m_title);
    painter->restore();
}


ChartItem::LegendAlign ChartItem::legendAlign() const
{
    return m_legendAlign;
}

void ChartItem::setLegendAlign(const LegendAlign &legendAlign)
{
    if (m_legendAlign != legendAlign){
        LegendAlign oldValue = m_legendAlign;
        m_legendAlign = legendAlign;
        notify("legendAlign",oldValue,m_legendAlign);
        update();
    }
}

bool ChartItem::drawLegendBorder() const
{
    return m_legendBorder;
}

void ChartItem::setDrawLegendBorder(bool legendBorder)
{
    if (m_legendBorder!=legendBorder){
        m_legendBorder = legendBorder;
        notify("legendBorder",!m_legendBorder,m_legendBorder);
        update();
    }
}

QString ChartItem::chartTitle() const
{
    return m_title;
}

void ChartItem::setChartTitle(const QString &title)
{
    if (m_title != title){
        QString oldValue = m_title;
        m_title = title;
        update();
        notify("chartTitle",oldValue,title);
    }
}

QList<SeriesItem *> &ChartItem::series()
{
    return m_series;
}

void ChartItem::setSeries(const QList<SeriesItem *> &series)
{
    m_series = series;
}

bool ChartItem::isSeriesExists(const QString &name)
{
    foreach (SeriesItem* series, m_series) {
        if (series->name().compare(name)==0) return true;
    }
    return false;
}

AbstractChart::AbstractChart(ChartItem *chartItem)
    :m_chartItem(chartItem)
{
    m_designLabels<<QObject::tr("First")<<QObject::tr("Second")<<QObject::tr("Thrid");
}

QRectF AbstractChart::calcChartLegendRect(const QFont &font, const QRectF &parentRect, bool takeAllRect, qreal borderMargin, qreal titleOffset)
{
    QSizeF legendSize = calcChartLegendSize(font);
    qreal legendTopMargin = 0;
    qreal legendBottomMargin = 0;

    switch (m_chartItem->legendAlign()) {
    case ChartItem::LegendAlignTop:
        legendTopMargin = titleOffset+borderMargin;
        legendBottomMargin = parentRect.height()-(legendSize.height()+titleOffset);
        break;
    case ChartItem::LegendAlignCenter:
        legendTopMargin = titleOffset+(parentRect.height()-titleOffset-legendSize.height())/2;
        legendBottomMargin = (parentRect.height()-titleOffset-legendSize.height())/2;
        break;
    case ChartItem::LegendAlignBottom:
            legendTopMargin = parentRect.height()-(legendSize.height()+titleOffset);
            legendBottomMargin = borderMargin;
        break;
    }

    qreal rightOffset = !takeAllRect?((legendSize.width()>parentRect.width()/2-borderMargin)?
                (parentRect.width()/2):
                (parentRect.width()-legendSize.width())):0;

    QRectF legendRect = parentRect.adjusted(
                    rightOffset,
                    (legendSize.height()>(parentRect.height()-titleOffset))?(titleOffset):(legendTopMargin),
                    -borderMargin,
                    (legendSize.height()>(parentRect.height()-titleOffset))?(0):(-legendBottomMargin)
                );

    return legendRect;
}

void AbstractChart::prepareLegendToPaint(QRectF &legendRect, QPainter *painter)
{
    QFont tmpFont = painter->font();
    QSizeF legendSize = calcChartLegendSize(tmpFont);

    if ((legendSize.height()>legendRect.height() || legendSize.width()>legendRect.width())){
        while ( (legendSize.height()>legendRect.height() || legendSize.width()>legendRect.width())
                && tmpFont.pixelSize()>1)
        {
            tmpFont.setPixelSize(tmpFont.pixelSize()-1);
            painter->setFont(tmpFont);
            legendSize = calcChartLegendSize(tmpFont);
        }
        painter->setFont(tmpFont);
        legendRect = calcChartLegendRect(tmpFont, legendRect, true, 0, 0);
    }
}

void PieChart::drawPercent(QPainter *painter, QRectF chartRect, qreal startAngle, qreal angle)
{
    painter->save();

    QPointF center(chartRect.left()+chartRect.width()/2,chartRect.top()+chartRect.height()/2);
    qreal percent = angle/3.6;
#ifdef HAVE_QT4
    qreal radAngle = (angle/2+startAngle)*(M_PI/180);
#endif
#ifdef HAVE_QT5
    qreal radAngle = qDegreesToRadians(angle/2+startAngle);
#endif
    qreal radius = painter->fontMetrics().width("99,9%");
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
    } else {
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
    } else {
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

QSizeF PieChart::calcChartLegendSize(const QFont &font)
{
    QFontMetrics fm(font);

    qreal cw = 0;
    qreal maxWidth = 0;

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
        SeriesItem* si = m_chartItem->series().at(0);
        foreach(QString label, si->data()->labels()){
            cw += fm.height();
            if (maxWidth<fm.width(label))
                maxWidth = fm.width(label)+10;
        }
    } else {
        foreach(QString label, m_designLabels){
            cw += fm.height();
            if (maxWidth<fm.width(label))
                maxWidth = fm.width(label)+10;
        }
    }
    cw += fm.height();
    return  QSizeF(maxWidth+fm.height()*2,cw);
}

int genNextValue(int value){
    int curValue = value;
    while (curValue%4!=0){
        curValue++;
    }
    return curValue;
}

qreal VerticalBarChart::valuesHMargin(QPainter* painter)
{
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);
    return painter->fontMetrics().width(QString::number(delta))+4;
}

qreal VerticalBarChart::valuesVMargin(QPainter *painter)
{
    return painter->fontMetrics().height();
}

QRectF VerticalBarChart::labelsRect(QPainter *painter, QRectF labelsRect)
{
    qreal maxWidth = 0;

    foreach (QString label, m_chartItem->labels()) {
        if (painter->fontMetrics().width(label)>maxWidth)
            maxWidth = painter->fontMetrics().width(label);
    }

    if (maxWidth+vPadding(m_chartItem->rect())> labelsRect.height())
        return labelsRect;
    else
        return labelsRect.adjusted(0,(labelsRect.height()-(maxWidth+vPadding(m_chartItem->rect()))),0,0);
}

void VerticalBarChart::paintChart(QPainter *painter, QRectF chartRect)
{
    QRectF calcRect = labelsRect(painter, chartRect.adjusted(
                                     hPadding(chartRect)*2+valuesHMargin(painter),
                                     chartRect.height()*0.5,
                                     -(hPadding(chartRect)*2),
                                     -vPadding(chartRect)
                                     ));

    qreal barsShift = calcRect.height();
    paintVerticalGrid(painter, chartRect.adjusted(
                  hPadding(chartRect),
                  vPadding(chartRect)+valuesVMargin(painter),
                  -hPadding(chartRect),-(vPadding(chartRect)+barsShift) ));

    paintVerticalBars(painter, chartRect.adjusted(
                  hPadding(chartRect)*2+valuesHMargin(painter),
                  vPadding(chartRect)+valuesVMargin(painter),
                  -(hPadding(chartRect)*2),
                  -(vPadding(chartRect)+barsShift) ));
    paintSerialLines(painter, chartRect.adjusted(
                         hPadding(chartRect)*2+valuesHMargin(painter),
                         vPadding(chartRect)+valuesVMargin(painter),
                         -(hPadding(chartRect)*2),
                         -(vPadding(chartRect)+barsShift) ));
    paintLabels(painter,calcRect);
}


void VerticalBarChart::paintVerticalGrid(QPainter *painter, QRectF gridRect)
{
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    painter->setRenderHint(QPainter::Antialiasing,false);
    qreal vStep = gridRect.height() / 4;

    for (int i=0;i<5;i++){
        painter->drawText(QRectF(gridRect.bottomLeft()-QPointF(0,vStep*i+painter->fontMetrics().height()),
                                 QSizeF(valuesHMargin(painter),painter->fontMetrics().height())),
                          QString::number(minValue()+i*delta/4));
        painter->drawLine(gridRect.bottomLeft()-QPointF(-valuesHMargin(painter),vStep*i),
                          gridRect.bottomRight()-QPointF(0,vStep*i));
    }

    painter->setRenderHint(QPainter::Antialiasing,true);
}



void VerticalBarChart::paintVerticalBars(QPainter *painter, QRectF barsRect)
{

    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    int barSeriesCount = 0;
    foreach(SeriesItem* series, m_chartItem->series()){
        if (series->preferredType() == SeriesItem::Bar) barSeriesCount++;
    }

    barSeriesCount = (m_chartItem->itemMode()==DesignMode) ? seriesCount() : barSeriesCount;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,false);

    qreal vStep = barsRect.height() / delta;
    qreal hStep = (barsRect.width() / valuesCount()) / (barSeriesCount == 0 ? 1 : barSeriesCount);
    qreal topShift = (delta - (maxValue()-minValue())) * vStep +barsRect.top();

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
        int curSeries = 0;
        foreach (SeriesItem* series, m_chartItem->series()) {
            if (series->preferredType() == SeriesItem::Bar){
                qreal curHOffset = curSeries*hStep+barsRect.left();
                painter->setBrush(series->color());
                foreach (qreal value, series->data()->values()) {
                    painter->drawRect(QRectF(curHOffset, maxValue()*vStep+topShift, hStep, -value*vStep));
                    curHOffset+=hStep*barSeriesCount;
                }
                curSeries++;
            }
        }
    } else {
        qreal curHOffset = barsRect.left();
        int curColor = 0;
        for (int i=0; i<9; ++i){
            if (curColor==3) curColor=0;
            painter->setBrush(color_map[curColor]);
            painter->drawRect(QRectF(curHOffset, maxValue()*vStep+barsRect.top(), hStep, -designValues()[i]*vStep));
            curHOffset+=hStep;
            curColor++;
        }
    }
    painter->restore();
}

void VerticalBarChart::paintSerialLines(QPainter* painter, QRectF barsRect)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    qreal vStep = barsRect.height() / delta;
    qreal hStep = (barsRect.width() / valuesCount());
    qreal topShift = (delta - (maxValue()-minValue())) * vStep +barsRect.top();

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
        foreach (SeriesItem* series, m_chartItem->series()) {
            if (series->preferredType() == SeriesItem::Line){
                QPen pen(series->color());
                pen.setWidth(4);
                painter->setPen(pen);
                for (int i = 0; i < series->data()->values().count()-1; ++i ){
                    QPoint startPoint = QPoint((i+1)*hStep + barsRect.left()-hStep/2,
                                               (maxValue()*vStep+topShift) - series->data()->values().at(i)*vStep
                                        );
                    QPoint endPoint = QPoint((i+2)*hStep + barsRect.left()-hStep/2,
                                             (maxValue()*vStep+topShift) - series->data()->values().at(i+1)*vStep
                                      );
                    painter->drawLine(startPoint, endPoint);
                    QRect startPointRect(startPoint,startPoint);
                    QRect endPointRect(endPoint,endPoint);
                    int radius = 4;
                    painter->setBrush(series->color());
                    painter->drawEllipse(startPointRect.adjusted(radius,radius,-radius,-radius));
                    painter->drawEllipse(endPointRect.adjusted(radius,radius,-radius,-radius));

                }
            }
        }
    }
    painter->restore();
}



void VerticalBarChart::paintLabels(QPainter *painter, QRectF labelsRect)
{
    painter->save();
    qreal hStep = (labelsRect.width() / valuesCount());

    if (!m_chartItem->labels().isEmpty()){
        painter->rotate(270);
        painter->translate(-(labelsRect.top()+labelsRect.height()),labelsRect.left());
        foreach (QString label, m_chartItem->labels()) {
            painter->drawText(QRectF(QPoint(0,0),
                                     QSize(labelsRect.height()-4,hStep)),Qt::AlignVCenter|Qt::AlignRight,label);
            painter->translate(0,hStep);
        }
        painter->rotate(-270);
    }
    painter->restore();
}

AbstractSeriesChart::AbstractSeriesChart(ChartItem *chartItem)
    :AbstractChart(chartItem)
{
    m_designValues[0] = 10;
    m_designValues[1] = 35;
    m_designValues[2] = 15;
    m_designValues[3] = 5;
    m_designValues[4] = 20;
    m_designValues[5] = 10;
    m_designValues[6] = 40;
    m_designValues[7] = 20;
    m_designValues[8] = 5;
}

qreal AbstractSeriesChart::maxValue()
{
    if (m_chartItem->itemMode()==DesignMode) return 40;
    qreal maxValue = 0;
    foreach(SeriesItem* series, m_chartItem->series()){
        foreach(qreal value, series->data()->values()){
            if (value>maxValue) maxValue=value;
        }
    }
    return maxValue;
}

qreal AbstractSeriesChart::minValue()
{
    if (m_chartItem->itemMode()==DesignMode) return 0;
    qreal minValue = 0;
    foreach(SeriesItem* series, m_chartItem->series()){
        foreach(qreal value, series->data()->values()){
            if (value<minValue) minValue=value;
        }
    }
    return minValue;
}

int AbstractSeriesChart::valuesCount()
{
    if (m_chartItem->itemMode()==DesignMode) return 3;
    return (m_chartItem->series().isEmpty())?(0):(m_chartItem->series().at(0)->data()->labels().count());
}

int AbstractSeriesChart::seriesCount()
{
    if (m_chartItem->itemMode()==DesignMode) return 3;
    return m_chartItem->series().count();
}

QSizeF AbstractSeriesChart::calcChartLegendSize(const QFont &font)
{
    QFontMetrics fm(font);

    qreal cw = 0;
    qreal maxWidth = 0;

    if (!m_chartItem->series().isEmpty()){
        foreach(SeriesItem* series, m_chartItem->series()){
            cw += fm.height();
            if (maxWidth<fm.width(series->name()))
                maxWidth = fm.width(series->name())+10;
        }
    } else {
        foreach(QString label, m_designLabels){
            cw += fm.height();
            if (maxWidth<fm.width(label))
                maxWidth = fm.width(label)+10;
        }
    }
    cw += fm.height();
    return  QSizeF(maxWidth+fm.height()*2,cw);
}

qreal HorizontalBarChart::valuesHMargin(QPainter *painter)
{
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);
    return painter->fontMetrics().width(QString::number(delta))+4;
}

qreal HorizontalBarChart::valuesVMargin(QPainter *painter)
{
    return painter->fontMetrics().height();
}

void HorizontalBarChart::paintChart(QPainter *painter, QRectF chartRect)
{
    QRectF calcRect = labelsRect(painter, chartRect.adjusted(
                                     hPadding(chartRect),
                                     vPadding(chartRect)*2,
                                     -(chartRect.width()*0.5),
                                     -(vPadding(chartRect)*2+valuesVMargin(painter))
                                     ));

    qreal barsShift = calcRect.width();

    paintHorizontalGrid(painter, chartRect.adjusted(
                  hPadding(chartRect)+barsShift,
                  vPadding(chartRect),
                  -(hPadding(chartRect)),-vPadding(chartRect)));
    paintHorizontalBars(painter, chartRect.adjusted(
                  hPadding(chartRect)+barsShift,
                  vPadding(chartRect)*2,
                  -(hPadding(chartRect)),
                  -(vPadding(chartRect)*2) ));

    paintLabels(painter,calcRect);
}

void HorizontalBarChart::paintHorizontalGrid(QPainter *painter, QRectF gridRect)
{
    painter->save();
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    painter->setRenderHint(QPainter::Antialiasing,false);
    qreal hStep = (gridRect.width()-painter->fontMetrics().width(QString::number(maxValue()))) / 4;

    painter->setFont(adaptValuesFont(hStep-4,painter->font()));

    for (int i=0;i<5;i++){
        painter->drawText(QRectF(gridRect.left()+4+hStep*i,gridRect.bottom()-painter->fontMetrics().height(),
                                 hStep,painter->fontMetrics().height()),
                          QString::number(minValue()+i*delta/4));
        painter->drawLine( gridRect.left()+hStep*i, gridRect.bottom(),
                           gridRect.left()+hStep*i, gridRect.top());

    }
    painter->restore();
}

void HorizontalBarChart::paintHorizontalBars(QPainter *painter, QRectF barsRect)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,false);
    int delta = int(maxValue()-minValue());
    delta = genNextValue(delta);

    qreal vStep = (barsRect.height()-painter->fontMetrics().height()) / valuesCount() / seriesCount();
    qreal hStep = (barsRect.width()-painter->fontMetrics().width(QString::number(maxValue()))) / delta;

    if (!m_chartItem->series().isEmpty() && !m_chartItem->series().at(0)->data()->labels().isEmpty()){
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

QRectF HorizontalBarChart::labelsRect(QPainter *painter, QRectF labelsRect)
{
    qreal maxWidth = 0;

    foreach (QString label, m_chartItem->labels()) {
        if (painter->fontMetrics().width(label)>maxWidth)
            maxWidth = painter->fontMetrics().width(label);
    }

    if (maxWidth+hPadding(m_chartItem->rect())*2> labelsRect.width())
        return labelsRect;
    else
        return labelsRect.adjusted(0,0,-(labelsRect.width()-(maxWidth+hPadding(m_chartItem->rect())*2)),0);
}

QFont HorizontalBarChart::adaptLabelsFont(QRectF rect, QFont font)
{
    QString maxWord;
    QFontMetrics fm(font);

    foreach(QString label, m_chartItem->labels()){
        foreach (QString currentWord, label.split(QRegExp("\\W+"))){
            if (fm.width(maxWord)<fm.width(currentWord)) maxWord = currentWord;
        }
    }

    qreal curWidth = fm.width(maxWord);
    QFont tmpFont = font;
    while (curWidth>rect.width() && tmpFont.pixelSize()>1){
        tmpFont.setPixelSize(tmpFont.pixelSize()-1);
        QFontMetricsF tmpFM(tmpFont);
        curWidth = tmpFM.width(maxWord);
    }
    return tmpFont;
}

QFont HorizontalBarChart::adaptValuesFont(qreal width, QFont font)
{
    QString strValue = QString::number(maxValue());
    QFont tmpFont = font;
    QScopedPointer<QFontMetricsF> fm(new QFontMetricsF(tmpFont));
    qreal curWidth = fm->width(strValue);
    while (curWidth>width && tmpFont.pixelSize()>1){
        tmpFont.setPixelSize(tmpFont.pixelSize()-1);
        fm.reset(new QFontMetricsF(tmpFont));
        curWidth = fm->width(strValue);
    }
    return tmpFont;
}

void HorizontalBarChart::paintLabels(QPainter *painter, QRectF labelsRect)
{
    painter->save();
    painter->setFont(adaptLabelsFont(labelsRect.adjusted(0,0,-hPadding(m_chartItem->rect()),0),
                               painter->font()));
    qreal vStep = (labelsRect.height() / valuesCount());
    int curLabel = 0;

    painter->translate(labelsRect.topLeft());
    if (!m_chartItem->labels().isEmpty()){
        foreach (QString label, m_chartItem->labels()) {
            painter->drawText(QRectF(QPoint(0,vStep*curLabel),
                                     QSize(labelsRect.width()-hPadding(m_chartItem->rect()),vStep)),
                              Qt::AlignVCenter | Qt::AlignRight | Qt::TextWordWrap,label);

            curLabel++;
        }
    }
    painter->restore();
}

qreal AbstractBarChart::hPadding(QRectF chartRect)
{
    return (chartRect.width()*0.02);
}

qreal AbstractBarChart::vPadding(QRectF chartRect)
{
    return (chartRect.height()*0.02);
}

void AbstractBarChart::paintChartLegend(QPainter *painter, QRectF legendRect)
{
    prepareLegendToPaint(legendRect, painter);
    int indicatorSize = painter->fontMetrics().height()/2;
    painter->setPen(Qt::black);
    painter->setRenderHint(QPainter::Antialiasing,false);
    if (m_chartItem->drawLegendBorder())
        painter->drawRect(legendRect);
    painter->setRenderHint(QPainter::Antialiasing,true);
    QRectF indicatorsRect = legendRect.adjusted(painter->fontMetrics().height()/2,painter->fontMetrics().height()/2,0,0);

    if (!m_chartItem->series().isEmpty()){
        qreal cw = 0;
        foreach(SeriesItem* series, m_chartItem->series()){
            QString label = series->name();
            painter->drawText(indicatorsRect.adjusted(indicatorSize+indicatorSize/2,cw,0,0),label);
            painter->setBrush(series->color());
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
    } else {
        qreal cw = 0;
        for (int i=0;i<m_designLabels.size();++i){
            QString label = m_designLabels.at(i);
            painter->drawText(indicatorsRect.adjusted(indicatorSize+indicatorSize/2,cw,0,0),label);
            painter->setBrush(color_map[i]);
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

} // namespace LimeReport
