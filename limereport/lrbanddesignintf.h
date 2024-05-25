/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#ifndef LRBANDDESIGNINTF_H
#define LRBANDDESIGNINTF_H
#include "lrbasedesignintf.h"
#include "lrdatasourcemanager.h"
#include "lritemscontainerdesignitf.h"
#include <QObject>

namespace LimeReport {

class IGroupBand
{
public:
    virtual void startGroup(DataSourceManager* dataManager) = 0;
    virtual bool isNeedToClose(DataSourceManager* dataManager) = 0;
    virtual bool isStarted() = 0;
    virtual void closeGroup() = 0;
    virtual int  index() = 0;
    virtual bool startNewPage() const = 0 ;
    virtual bool resetPageNumber() const = 0 ;
    virtual ~IGroupBand(){}
};

class BandDesignIntf;

class BandMarker : public QGraphicsItem{
public:
    explicit BandMarker(BandDesignIntf* band, QGraphicsItem *parent=0);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setHeight(qreal height);
    void setWidth(qreal width);
    void setColor(QColor color);
    qreal width(){return m_rect.width();}
    qreal height(){return m_rect.height();}
protected:
    void  mousePressEvent(QGraphicsSceneMouseEvent *event);
    void  contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void  hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void  mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    QRectF m_rect;
    QColor m_color;
    BandDesignIntf* m_band;
    QPointF m_oldBandPos;
};

class BandNameLabel : public QGraphicsItem{
public:
    explicit BandNameLabel(BandDesignIntf* band, QGraphicsItem* parent=0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void updateLabel(const QString &bandName);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
private:
    QRectF m_rect;
    QColor m_color;
    BandDesignIntf* m_band;
};

class InitializedValue{
public:
    InitializedValue(): m_value(-1), m_isInitialized(false){}
    InitializedValue(qreal value): m_value(value), m_isInitialized(true){}
    qreal   value() const { return m_value;}
    void    setValue( qreal value){ m_value = value; m_isInitialized = true;}
    bool    isValid() const{ return m_isInitialized;}
private:
    qreal   m_value;
    bool    m_isInitialized;
};

class BandDesignIntf : public ItemsContainerDesignInft
{
    Q_OBJECT
    Q_PROPERTY(bool autoHeight READ autoHeight WRITE setAutoHeight)
    Q_PROPERTY(int bandIndex READ bandIndex WRITE setBandIndex DESIGNABLE false)
    Q_PROPERTY(bool keepBottomSpace READ keepBottomSpace WRITE setKeepBottomSpace)
    Q_PROPERTY(bool keepTopSpace READ keepTopSpace WRITE setKeepTopSpace)
    Q_PROPERTY(QString parentBand READ parentBandName WRITE setParentBandName DESIGNABLE false)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(BrushStyle backgroundBrushStyle READ backgroundBrushStyle WRITE setBackgroundBrushStyle)
    Q_PROPERTY(bool printIfEmpty READ printIfEmpty WRITE setPrintIfEmpty)
    Q_PROPERTY(BGMode backgroundMode READ backgroundMode WRITE setBackgroundModeProperty)
    Q_PROPERTY(int backgroundOpacity READ opacity WRITE setBackgroundOpacity)
    Q_PROPERTY(int shiftItems READ shiftItems WRITE setShiftItems)
    friend class BandMarker;
    friend class BandNameLabel;
    friend class ReportRender;
public:

    enum BandsType {
        PageHeader=0,
        ReportHeader=1,
        DataHeader=2,
        GroupHeader=3,
        Data=4,
        SubDetailHeader=5,
        SubDetailBand=6,
        SubDetailFooter=7,
        GroupFooter=8,
        DataFooter=9,
        ReportFooter=10,
        TearOffBand=11,
        PageFooter=12
    };

    enum BandColumnsLayoutType{
        Horizontal, Vertical, VerticalUniform
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(BandColumnsLayoutType)
#else
    Q_ENUMS(BandColumnsLayoutType)
#endif

    BandDesignIntf(BandsType bandType, const QString& xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0);
    ~BandDesignIntf();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    void translateBandsName();
    virtual BandsType bandType() const;
    virtual QString bandTitle() const;
    virtual QIcon bandIcon() const;
    virtual bool isUnique() const;
    void setItemMode(BaseDesignIntf::ItemMode mode);
    void updateItemSize(DataSourceManager *dataManager, RenderPass pass=FirstPass, int maxHeight=0);
    void updateBandNameLabel();
    void initFromItem(BaseDesignIntf* source);

    virtual QColor selectionColor() const;
    int bandIndex() const;
    void setBandIndex(int value);
    void changeBandIndex(int value, bool firstTime = false);
    void setBandType(BandsType value){m_bandType=value;}

    QString datasourceName();
    void setDataSourceName(const QString& datasourceName);

    void setKeepBottomSpace(bool value);
    bool keepBottomSpace() const {return m_keepBottomSpace;}

    bool keepTopSpace() const;
    void setKeepTopSpace(bool value);

    void addChildBand(BandDesignIntf* band);
    bool hasChildren(){return !m_childBands.isEmpty();}
    void removeChildBand(BandDesignIntf* band);
    void setParentBand(BandDesignIntf* band);

    void setParentBandName(const QString& parentBandName);
    QString parentBandName();

    bool isConnectedToBand(BandDesignIntf::BandsType bandType) const;

    int minChildIndex(BandsType bandType);
    int minChildIndex(QSet<BandsType> ignoredBands = QSet<BandDesignIntf::BandsType>());
    int maxChildIndex(BandDesignIntf::BandsType bandType) const;
    int maxChildIndex(QSet<BandsType> ignoredBands = QSet<BandDesignIntf::BandsType>()) const;

    int rootIndex(BandDesignIntf *parentBand);
    BandDesignIntf* rootBand(BandDesignIntf *parentBand);

    BandDesignIntf* parentBand() const {return m_parentBand;}

    QList<BandDesignIntf*> childBands() const{return m_childBands;}
    QList<BandDesignIntf*> childrenByType(BandDesignIntf::BandsType type);

    bool canBeSplitted(int height) const;
    bool isEmpty() const;
    virtual bool isNeedRender() const;
    virtual bool isFooter() const {return false;}
    virtual bool isHeader() const {return false;}
    virtual bool isGroupHeader() const {return false;}
    virtual bool isData() const {return false;}
    virtual int  bandNestingLevel(){return 0;}
    bool isBand(){return true;}

    void setTryToKeepTogether(bool value);
    bool tryToKeepTogether();

    BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner=0, QGraphicsItem *parent=0);
    void parentObjectLoadFinished();
    void objectLoadFinished();
    void emitBandRendered(BandDesignIntf *band);
    void emitBandReRendered(BandDesignIntf* oldBand, BandDesignIntf* newBand);

    bool isSplittable() const {return m_splitable;}
    void setSplittable(bool value);

    bool keepFooterTogether() const;
    void setKeepFooterTogether(bool value);

    int maxScalePercent() const;
    void setMaxScalePercent(int maxScalePercent);

    bool sliceLastRow() const;
    void setSliceLastRow(bool sliceLastRow);

    bool printIfEmpty() const;
    void setPrintIfEmpty(bool printIfEmpty);

    virtual BandDesignIntf* bandHeader();
    virtual BandDesignIntf* bandFooter();

    int columnsCount() const {return m_columnsCount;}
    BandColumnsLayoutType columnsFillDirection(){ return m_columnsFillDirection;}
    int columnIndex() const;
    void setColumnIndex(int columnIndex);
    
    bool reprintOnEachPage() const;
    void setReprintOnEachPage(bool reprintOnEachPage);

    bool startNewPage() const;
    void setStartNewPage(bool startNewPage);

    void setAutoHeight(bool value);
    bool autoHeight(){return m_autoHeight;}

    bool startFromNewPage() const;
    void setStartFromNewPage(bool startFromNewPage);
    bool canContainChildren() const{ return true;}
    bool canAcceptPaste() const{ return true;}
    bool printAlways() const;
    void setPrintAlways(bool printAlways);
    bool repeatOnEachRow() const;
    void setRepeatOnEachRow(bool repeatOnEachRow);
    QColor alternateBackgroundColor() const;
    void setAlternateBackgroundColor(const QColor &alternateBackgroundColor);
    bool useAlternateBackgroundColor() const;
    void setUseAlternateBackgroundColor(bool useAlternateBackgroundColor);    
    qreal bottomSpace();
    void setBottomSpace(qreal bottomSpace);
    void setBackgroundModeProperty(BGMode value);
    void setBackgroundOpacity(int value);
    void updateBandMarkerGeometry();
    int shiftItems() const;
    void setShiftItems(int shiftItems);    
    bool isNeedUpdateSize(RenderPass) const;
    void copyBandAttributes(BandDesignIntf* source);
signals:
    void bandRendered(BandDesignIntf* band);
    void bandReRendered(BandDesignIntf* oldBand, BandDesignIntf* newBand);
    void preparedForRender();
    void bandRegistred();
protected:
    void  trimToMaxHeight(int maxHeight);
    void  setBandTypeText(const QString& value);
    QString bandTypeText(){return m_bandTypeText;}
    void moveDown(){}
    void moveUp(){}
    QSet<BandsType> groupBands();
    QSet<BandsType> subdetailBands();
    BandDesignIntf *findParentBand();
    void geometryChangedEvent(QRectF, QRectF);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void initMode(ItemMode mode);
    virtual QColor bandColor() const;
    void setMarkerColor(QColor color);
    void checkEmptyTable();
    void setColumnsCount(int value);
    void setColumnsFillDirection(BandColumnsLayoutType value);
    void moveItemsDown(qreal startPos, qreal offset);
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
    QString translateBandName(const BaseDesignIntf *item) const;

private slots:
    void childBandDeleted(QObject* band);
    void slotPropertyObjectNameChanged(const QString&,const QString&);
private:
    QString                     m_bandTypeText;
    BandsType                   m_bandType;
    int                         m_bandIndex;
    QString                     m_dataSourceName;
    bool                        m_autoHeight;
    bool                        m_keepBottomSpace;
    bool                        m_keepTopSpace;
    BandDesignIntf*             m_parentBand;
    QString                     m_parentBandName;
    QList<BandDesignIntf*>      m_childBands;
    QVector<PItemSortContainer> m_bandItems;
    BandMarker*                 m_bandMarker;
    bool                        m_tryToKeepTogether;
    bool                        m_splitable;
    bool                        m_keepFooterTogether;
    int                         m_maxScalePercent;
    bool                        m_sliceLastRow;
    bool                        m_printIfEmpty;
    BandNameLabel*              m_bandNameLabel;
    int                         m_columnsCount;
    int                         m_columnIndex;
    BandColumnsLayoutType       m_columnsFillDirection;
    bool                        m_reprintOnEachPage;
    bool                        m_startNewPage;
    bool                        m_startFromNewPage;
    bool                        m_printAlways;
    bool                        m_repeatOnEachRow;
    QMap<QString,BaseDesignIntf*> m_slicedItems;
    QColor 						m_alternateBackgroundColor;
    bool                        m_useAlternateBackgroundColor;
    int 						m_bottomSpace;
    QMap<QString,QVariant>      m_bookmarks;
    int                         m_shiftItems;
};

class DataBandDesignIntf : public BandDesignIntf{
    Q_OBJECT
    Q_PROPERTY(QString datasource READ datasourceName WRITE setDataSourceName)
public:
    DataBandDesignIntf(BandsType bandType, QString xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0);
};

bool bandIndexLessThen(const BandDesignIntf* b1, const BandDesignIntf* b2);

} // namespace LimeReport
#endif // LRBANDDESIGNINTF_H
