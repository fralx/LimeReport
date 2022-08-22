#ifndef LRCHARTITEM_H
#define LRCHARTITEM_H
#include "lritemdesignintf.h"
#include "lrglobal.h"
#include "lraxisdata.h"
#include <QtGlobal>

namespace LimeReport{

QColor generateColor();
extern QColor color_map[39];

class IDataSource;

class SeriesItemData : public QObject{
    Q_OBJECT
public:
    QList<qreal>& values(){ return m_values;}
    QList<qreal>& xAxisValues(){ return m_xAxisValues;}
    QList<QString>& labels(){ return m_labels;}
    QList<QColor>& colors() { return m_colors;}
    void clear(){ m_values.clear(); m_labels.clear(); m_colors.clear(); }
private:
    QList<qreal> m_values, m_xAxisValues;
    QList<QString> m_labels;
    QList<QColor> m_colors;
};

class SeriesItem : public QObject{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString valuesColumn READ valuesColumn WRITE setValuesColumn)
    Q_PROPERTY(QString labelsColumn READ labelsColumn WRITE setLabelsColumn)
    Q_PROPERTY(QString xAxisColumn READ xAxisColumn WRITE setXAxisColumn)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(SeriesItemPreferredType preferredType READ preferredType WRITE setPreferredType)
public:
    enum SeriesItemPreferredType {Bar, Line};
#if QT_VERSION >= 0x050500
    Q_ENUM(SeriesItemPreferredType)
#else
    Q_ENUMS(SeriesItemPreferredType)
#endif
    SeriesItem(QObject* parent = 0) : QObject(parent), m_preferredType(Bar){}
    QString name() const;
    void setName(const QString &name);
    QString valuesColumn() const;
    void setValuesColumn(const QString &valuesColumn);
    QString labelsColumn() const;
    void setLabelsColumn(const QString &labelsColumn);
    QString xAxisColumn() const;
    void setXAxisColumn(const QString &xAxisColumn);
    SeriesItem* clone();
    void fillSeriesData(IDataSource* dataSource);
    SeriesItemData* data(){ return &m_data;}
    QColor color() const;
    void setColor(const QColor &color);
    SeriesItemPreferredType preferredType() const;
    void setPreferredType(const SeriesItemPreferredType& preferredType);
    bool isEmpty(){ return m_data.values().isEmpty();}
private:
    QString m_name;
    QString m_valuesColumn;
    QString m_labelsColumn;
    QString m_xAxisColumn;
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
    virtual QSizeF calcChartLegendSize(const QFont &font, qreal maxWidth = 0) = 0;
    virtual QRectF calcChartLegendRect(const QFont& font, const QRectF& parentRect, bool takeAllRect, qreal borderMargin, qreal titleOffset);

    QFont titleFont();
    void setTitleFont(const QFont &value);
protected:
    QVector<qreal> legendColumnWidths() const;
    virtual void prepareLegendToPaint(QRectF& legendRect, QPainter *painter);
protected:
    // Title font must be placed here instead of CharItem, becuase
    // it would cause crash when creating CharItem object on embedded
    QFont m_titleFont;
    ChartItem* m_chartItem;
    QList<QString> m_designLabels;
    QVector<qreal> m_legendColumnWidths;
};

class AbstractSeriesChart: public AbstractChart{
public:
    AbstractSeriesChart(ChartItem* chartItem);
protected:
    AxisData &xAxisData() const;
    AxisData &yAxisData() const;
    qreal maxValue();
    qreal minValue();
    void updateMinAndMaxValues();
    int valuesCount();
    int seriesCount();
    bool verticalLabels(QPainter* painter, QRectF labelsRect);
    QSizeF calcChartLegendSize(const QFont &font, qreal maxWidth);
    qreal* designValues(){ return m_designValues;}
    virtual qreal hPadding(QRectF chartRect);
    virtual qreal vPadding(QRectF chartRect);
    virtual void paintHorizontalLabels(QPainter *painter, QRectF labelsRect);
    virtual void paintVerticalLabels(QPainter *painter, QRectF labelsRect);
    virtual void paintHorizontalGrid(QPainter *painter, QRectF gridRect);
    virtual void paintGrid(QPainter *painter, QRectF gridRect);
    virtual void paintVerticalGrid(QPainter *painter, QRectF gridRect);
    virtual void drawSegment(QPainter *painter, QPoint startPoint, QPoint endPoint, QColor color);
    virtual qreal valuesHMargin(QPainter *painter);
    virtual qreal valuesVMargin(QPainter *painter);
    virtual QFont adaptLabelsFont(QRectF rect, QFont font);
    virtual QFont adaptFont(qreal width, QFont font, const AxisData &axisData);
    virtual QString axisLabel(int i, const AxisData &axisData);

private:
    bool calculateLegendColumnWidths(qreal indicatorWidth, qreal maxWidth, const QFontMetrics &fm);
    bool calculateLegendSingleColumnWidth(qreal &currentRowWidth, int &currentColumn, int &maxColumnCount,
                                          const qreal itemWidth, const qreal maxRowWidth);
    qreal m_designValues [9];
};

class AbstractBarChart: public AbstractSeriesChart{
public:
    AbstractBarChart(ChartItem* chartItem):AbstractSeriesChart(chartItem){}
    void paintChartLegend(QPainter *painter, QRectF legendRect);
protected:
    QRectF verticalLabelsRect(QPainter* painter, QRectF horizontalLabelsRect);
    virtual QRectF horizontalLabelsRect(QPainter* painter, QRectF horizontalLabelsRect);
private:
    void drawVerticalLegendItem(QPainter *painter, int i, const QString &text,
                                int indicatorSize, const QRectF &indicatorsRect, const QColor &indicatorColor);
    void drawHorizontalLegendItem(QPainter *painter, int i, const QString &text,
                                  int indicatorSize, const QRectF &indicatorsRect, const QColor &indicatorColor);
};

class ChartItem : public LimeReport::ItemDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(QObject* xAxisSettings READ xAxisSettings WRITE setXAxisSettings)
    Q_PROPERTY(QObject* yAxisSettings READ yAxisSettings WRITE setYAxisSettings)
    Q_PROPERTY(ACollectionProperty series READ fakeCollectionReader WRITE setSeries)
    Q_PROPERTY(QString datasource READ datasource WRITE setDatasource)
    Q_PROPERTY(QString chartTitle READ chartTitle WRITE setChartTitle)
    Q_PROPERTY(bool drawLegendBorder READ drawLegendBorder WRITE setDrawLegendBorder)
    Q_PROPERTY(LegendAlign legendAlign READ legendAlign WRITE setLegendAlign)
    Q_PROPERTY(LegendStyle legendStyle READ legendStyle WRITE setLegendStyle)
    Q_PROPERTY(TitleAlign titleAlign READ titleAlign WRITE setTitleAlign)
    Q_PROPERTY(ChartType chartType READ chartType WRITE setChartType)
    Q_PROPERTY(QString labelsField READ labelsField WRITE setLabelsField)
    Q_PROPERTY(bool showLegend READ showLegend WRITE setShowLegend)
    Q_PROPERTY(QFont titleFont READ titleFont WRITE setTitleFont)
    Q_PROPERTY(QFont font READ font WRITE setCharItemFont)

    //linesChart
    Q_PROPERTY(bool drawPoints READ drawPoints WRITE setDrawPoints)
    Q_PROPERTY(int seriesLineWidth READ seriesLineWidth WRITE setSeriesLineWidth)
    Q_PROPERTY(bool horizontalAxisOnTop READ horizontalAxisOnTop WRITE setHorizontalAxisOnTop)

    //gridChart
    Q_FLAGS(GridChartLines)
    Q_PROPERTY(QString xAxisField READ xAxisField WRITE setXAxisField)
    Q_PROPERTY(GridChartLines gridChartLines READ gridChartLines WRITE setGridChartLines)
    friend class AbstractChart;
public:

    enum LegendAlign{LegendAlignRightTop,LegendAlignRightCenter,LegendAlignRightBottom,
                       LegendAlignBottomLeft,LegendAlignBottomCenter,LegendAlignBottomRight};
    enum LegendStyle{LegendPoints, LegendLines};
    enum TitleAlign{TitleAlignLeft, TitleAlignCenter, TitleAlignRight};
    enum ChartType{Pie, VerticalBar, HorizontalBar, Lines, GridLines};
    enum LineType {
        NoLine = 0,
        HorizontalLine = 1,
        VerticalLine = 2,
        AllLines = 3
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(LegendAlign)
    Q_ENUM(LegendStyle)
    Q_ENUM(TitleAlign)
    Q_ENUM(ChartType)
    Q_ENUM(LineType)
#else
    Q_ENUMS(LegendAlign)
    Q_ENUMS(LegendStyle)
    Q_ENUMS(TitleAlign)
    Q_ENUMS(ChartType)
    Q_ENUMS(LineType)
#endif
    Q_DECLARE_FLAGS(GridChartLines, LineType)

    ChartItem(QObject* owner, QGraphicsItem* parent);
    ~ChartItem();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QObject* xAxisSettings();
    void setYAxisSettings(QObject *axis);
    QObject* yAxisSettings();
    void setXAxisSettings(QObject *axis);

    AxisData *xAxisData();
    AxisData *yAxisData();

    void showAxisEditorDialog(bool isXAxis);

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

    LegendStyle legendStyle() const;
    void setLegendStyle(const LegendStyle &legendStyle);

    TitleAlign titleAlign() const;
    void setTitleAlign(const TitleAlign &titleAlign);

    ChartType chartType() const;
    void setChartType(const ChartType &chartType);

    QString labelsField() const;
    void setLabelsField(const QString &labelsField);

    QList<QString> labels() const;
    void setLabels(const QList<QString> &labels);
    QWidget* defaultEditor();

    bool showLegend() const;
    void setShowLegend(bool showLegend);

    bool drawPoints() const;
    void setDrawPoints(bool drawPoints);

    int seriesLineWidth() const;
    void setSeriesLineWidth(int newSeriesLineWidth);

    QString xAxisField() const;
    void setXAxisField(const QString &xAxisField);

    bool horizontalAxisOnTop() const;
    void setHorizontalAxisOnTop(bool horizontalAxisOnTop);

    GridChartLines gridChartLines() const;
    void setGridChartLines(GridChartLines flags);

    QFont titleFont() const;
    void setTitleFont(QFont value);
    void setCharItemFont(QFont value);

    QSettings *settings();

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
    bool m_isEmpty;
    bool m_showLegend;
    bool m_drawPoints;
    int m_seriesLineWidth;
    QString m_xAxisField;
    bool m_horizontalAxisOnTop;
    GridChartLines m_gridChartLines;
    LegendStyle m_legendStyle;
    AxisData *m_xAxisData, *m_yAxisData;
};
} //namespace LimeReport
#endif // LRCHARTITEM_H
