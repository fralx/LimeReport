#ifndef LRCHARTITEM_H
#define LRCHARTITEM_H
#include "lritemdesignintf.h"
#include "lrglobal.h"

namespace LimeReport{

QColor generateColor();
extern QColor color_map[39];

class IDataSource;

class SeriesItemData : public QObject{
    Q_OBJECT
public:
    QList<qreal>& values(){ return m_values;}
    QList<QString>& labels(){ return m_labels;}
    QList<QColor>& colors() { return m_colors;}
    void clear(){ m_values.clear(); m_labels.clear(); m_colors.clear(); }
private:
    QList<qreal> m_values;
    QList<QString> m_labels;
    QList<QColor> m_colors;
};

class SeriesItem : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString valuesColumn READ valuesColumn WRITE setValuesColumn )
    Q_PROPERTY(QString labelsColumn READ labelsColumn WRITE setLabelsColumn )
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(SeriesItemPreferredType preferredType READ preferredType WRITE setPreferredType)
    Q_ENUMS(SeriesItemPreferredType)
public:
    enum SeriesItemPreferredType {Bar, Line};
    SeriesItem(QObject* parent = 0) : QObject(parent), m_preferredType(Bar){}
    QString name() const;
    void setName(const QString &name);
    QString valuesColumn() const;
    void setValuesColumn(const QString &valuesColumn);
    QString labelsColumn() const;
    void setLabelsColumn(const QString &labelsColumn);
    SeriesItem* clone();
    void fillSeriesData(IDataSource* dataSource);
    SeriesItemData* data(){ return &m_data;}
    QColor color() const;
    void setColor(const QColor &color);
    SeriesItemPreferredType preferredType() const;
    void setPreferredType(const SeriesItemPreferredType& preferredType);
private:
    QString m_name;
    QString m_valuesColumn;
    QString m_labelsColumn;
    SeriesItemData m_data;
    QColor m_color;
    SeriesItemPreferredType m_preferredType;
};

class ChartItem;

class AbstractChart {
public:
    AbstractChart(ChartItem* chartItem);
    virtual ~AbstractChart(){}
    virtual void paintChart(QPainter *painter, QRectF rect) = 0;
    virtual void paintChartLegend(QPainter *painter, QRectF legendRect) =0;
    virtual QSizeF calcChartLegendSize(const QFont &font) = 0;
    virtual QRectF calcChartLegendRect(const QFont& font, const QRectF& parentRect, bool takeAllRect, qreal borderMargin, qreal titleOffset);
protected:
    virtual void prepareLegendToPaint(QRectF& legendRect, QPainter *painter);
protected:
    ChartItem* m_chartItem;
    QList<QString> m_designLabels;
};

class AbstractSeriesChart: public AbstractChart{
public:
    AbstractSeriesChart(ChartItem* chartItem);
protected:
    qreal maxValue();
    qreal minValue();
    int valuesCount();
    int seriesCount();
    bool verticalLabels(QPainter* painter, QRectF labelsRect);
    QSizeF calcChartLegendSize(const QFont &font);
    qreal* designValues(){ return m_designValues;}
    virtual qreal hPadding(QRectF chartRect);
    virtual qreal vPadding(QRectF chartRect);
    virtual void paintHorizontalLabels(QPainter *painter, QRectF labelsRect);
    virtual void paintVerticalLabels(QPainter *painter, QRectF labelsRect);
    virtual void paintHorizontalGrid(QPainter *painter, QRectF gridRect);
    virtual void paintVerticalGrid(QPainter *painter, QRectF gridRect);
    virtual void drawSegment(QPainter *painter, QPoint startPoint, QPoint endPoint, QColor color);
    virtual qreal valuesHMargin(QPainter *painter);
    virtual qreal valuesVMargin(QPainter *painter);
    virtual QFont adaptLabelsFont(QRectF rect, QFont font);
    virtual QFont adaptValuesFont(qreal width, QFont font);

private:
    qreal m_designValues [9];
};

int genNextValue(int value);

class AbstractBarChart: public AbstractSeriesChart{
public:
    AbstractBarChart(ChartItem* chartItem):AbstractSeriesChart(chartItem){}
    void paintChartLegend(QPainter *painter, QRectF legendRect);
protected:
    QRectF verticalLabelsRect(QPainter* painter, QRectF horizontalLabelsRect);
    virtual QRectF horizontalLabelsRect(QPainter* painter, QRectF horizontalLabelsRect);
};

class ChartItem : public LimeReport::ItemDesignIntf
{
    Q_OBJECT
    Q_ENUMS(LegendAlign)
    Q_ENUMS(TitleAlign)
    Q_ENUMS(ChartType)
    Q_PROPERTY(ACollectionProperty series READ fakeCollectionReader WRITE setSeries)
    Q_PROPERTY(QString datasource READ datasource WRITE setDatasource)
    Q_PROPERTY(QString chartTitle READ chartTitle WRITE setChartTitle)
    Q_PROPERTY(bool drawLegendBorder READ drawLegendBorder WRITE setDrawLegendBorder)
    Q_PROPERTY(LegendAlign legendAlign READ legendAlign WRITE setLegendAlign)
    Q_PROPERTY(TitleAlign titleAlign READ titleAlign WRITE setTitleAlign)
    Q_PROPERTY(ChartType chartType READ chartType WRITE setChartType)
    Q_PROPERTY(QString labelsField READ labelsField WRITE setLabelsField)
    friend class AbstractChart;
public:

    enum LegendAlign{LegendAlignTop,LegendAlignCenter,LegendAlignBottom};
    enum TitleAlign{TitleAlignLeft, TitleAlignCenter, TitleAlignRight};
    enum ChartType{Pie, VerticalBar, HorizontalBar, Lines};

    ChartItem(QObject* owner, QGraphicsItem* parent);
    ~ChartItem();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QList<SeriesItem *> &series();
    void setSeries(const QList<SeriesItem *> &series);
    bool isSeriesExists(const QString& name);

    QString datasource() const;
    void setDatasource(const QString &datasource);

    QString chartTitle() const;
    void setChartTitle(const QString &chartTitle);

    bool drawLegendBorder() const;
    void setDrawLegendBorder(bool drawLegendBorder);

    LegendAlign legendAlign() const;
    void setLegendAlign(const LegendAlign &legendAlign);

    TitleAlign titleAlign() const;
    void setTitleAlign(const TitleAlign &titleAlign);

    ChartType chartType() const;
    void setChartType(const ChartType &chartType);

    QString labelsField() const;
    void setLabelsField(const QString &labelsField);

    QList<QString> labels() const;
    void setLabels(const QList<QString> &labels);
    QWidget* defaultEditor();

protected:
    void paintChartTitle(QPainter* painter, QRectF titleRect);
    virtual BaseDesignIntf* createSameTypeItem(QObject *owner, QGraphicsItem *parent);
    //ICollectionContainer
    QObject* createElement(const QString& collectionName, const QString& elementType);
    int elementsCount(const QString& collectionName);
    QObject* elementAt(const QString& collectionName,int index);
    void collectionLoadFinished(const QString& collectionName){Q_UNUSED(collectionName)}    
    void updateItemSize(DataSourceManager *dataManager, RenderPass, int);
    void fillLabels(IDataSource* dataSource);    
    bool isNeedUpdateSize(RenderPass pass) const;
    void setSeries(ACollectionProperty series){Q_UNUSED(series)}
private:
    QList<SeriesItem*> m_series;
    QString m_datasource;
    QPixmap m_chartImage;
    QString m_title;
    AbstractChart* m_chart;
    bool m_legendBorder;
    LegendAlign m_legendAlign;
    TitleAlign  m_titleAlign;
    ChartType   m_chartType;
    QString     m_labelsField;
    QList<QString> m_labels;
};

} //namespace LimeReport
#endif // LRCHARTITEM_H
