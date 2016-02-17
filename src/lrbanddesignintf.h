/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
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
#include <QObject>

namespace LimeReport {

class IGroupBand
{
public:
    virtual void startGroup()=0;
    virtual bool isNeedToClose()=0;
    virtual bool isStarted()=0;
    virtual void closeGroup()=0;
    virtual int  index()=0;
    virtual ~IGroupBand(){}
};

class BandDesignIntf;

class BandMarker : public QGraphicsItem{
public:
    explicit BandMarker(BandDesignIntf* band, QGraphicsItem *parent=0);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    void setHeight(qreal height);
    void setWidth(qreal width);
    void setColor(QColor color);
    qreal width(){return m_rect.width();}
    qreal height(){return m_rect.height();}
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    QRectF m_rect;
    QColor m_color;
    BandDesignIntf* m_band;
};

struct ItemSortContainer;
typedef QSharedPointer< ItemSortContainer > PItemSortContainer;
class BandDesignIntf : public BaseDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(bool autoHeight READ autoHeight WRITE setAutoHeight )
    Q_PROPERTY(int bandIndex READ bandIndex WRITE setBandIndex DESIGNABLE false )
    Q_PROPERTY(bool keepBottomSpace READ keepBottomSpaceOption WRITE setKeepBottomSpaceOption )
    Q_PROPERTY(QString parentBand READ parentBandName WRITE setParentBandName DESIGNABLE false )
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(bool printIfEmpty READ printIfEmpty WRITE setPrintIfEmpty)
    friend class BandMarker;
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
        PageFooter=11
    };

    BandDesignIntf(BandsType bandType, const QString& xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0);
    ~BandDesignIntf();

    virtual void paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual BandsType bandType() const;
    virtual QString bandTitle() const;
    virtual QIcon bandIcon() const;
    virtual bool isUnique() const;
    virtual void updateItemSize(RenderPass pass=FirstPass, int maxHeight=0);

    virtual QColor selectionColor() const;
    int bandIndex() const;
    void setBandIndex(int value);

    void setBandType(BandsType value){m_bandType=value;}

    QString datasourceName();
    void setDataSourceName(const QString& datasourceName);

    void setKeepBottomSpaceOption(bool value){m_keepBottomSpace=value;}
    bool keepBottomSpaceOption() const {return m_keepBottomSpace;}

    void addChildBand(BandDesignIntf* band);
    bool hasChildren(){return !m_childBands.isEmpty();}
    void removeChildBand(BandDesignIntf* band);
    void setParentBand(BandDesignIntf* band);

    void setParentBandName(const QString& parentBandName);
    QString parentBandName(){return m_parentBandName;}

    bool isConnectedToBand(BandDesignIntf::BandsType bandType) const;

    int minChildIndex(BandsType bandType);
    int maxChildIndex(QSet<BandsType> ignoredBands = QSet<BandDesignIntf::BandsType>()) const;

    BandDesignIntf* parentBand() const {return m_parentBand;}

    QList<BandDesignIntf*> childBands() const{return m_childBands;}
    QList<BandDesignIntf*> childrenByType(BandDesignIntf::BandsType type);

    virtual bool canBeSplitted(int height) const;
    virtual bool isEmpty() const;
    virtual bool isNeedRender() const;
    virtual bool isFooter() const {return false;}
    virtual bool isHeader() const {return false;}
    virtual bool isData() const {return false;}

    void setTryToKeepTogether(bool value);
    bool tryToKeepTogether();

    BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner=0, QGraphicsItem *parent=0);
    void parentObjectLoadFinished();
    void objectLoadFinished();
    void emitBandRendered(BandDesignIntf *band);

    bool isSplittable() const {return m_splitable;}
    void setSplittable(bool value){m_splitable=value;}

    bool keepFooterTogether() const;
    void setKeepFooterTogether(bool keepFooterTogether);

    int maxScalePercent() const;
    void setMaxScalePercent(int maxScalePercent);

    bool sliceLastRow() const;
    void setSliceLastRow(bool sliceLastRow);

    bool printIfEmpty() const;
    void setPrintIfEmpty(bool printIfEmpty);

    virtual BandDesignIntf* bandHeader();
    virtual BandDesignIntf* bandFooter();

signals:
    void bandRendered(BandDesignIntf* band);
protected:
    void  snapshotItemsLayout();
    void  arrangeSubItems(RenderPass pass, ArrangeType type = AsNeeded);
    qreal findMaxBottom();
    qreal findMaxHeight();
    void  trimToMaxHeight(int maxHeight);

    void setAutoHeight(bool value){m_autoHeight=value;}
    bool autoHeight(){return m_autoHeight;}

    void setBandTypeText(const QString& value){m_bandTypeText=value;}
    QString bandTypeText(){return m_bandTypeText;}
    virtual void moveDown(){}
    virtual void moveUp(){}
    QSet<BandsType> groupBands();
    QSet<BandsType> subdetailBands();
    BandDesignIntf *findParentBand();
    virtual void geometryChangedEvent(QRectF, QRectF);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void initMode(ItemMode mode);
    virtual QColor bandColor() const;
    void setMarkerColor(QColor color);
    void checkEmptyTable();
private slots:
    void childBandDeleted(QObject* band);
private:
    QString                     m_bandTypeText;
    BandsType                   m_bandType;
    int                         m_bandIndex;
    QString                     m_dataSourceName;
    bool                        m_autoHeight;
    bool                        m_keepBottomSpace;
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
};

class DataBandDesignIntf : public BandDesignIntf{
    Q_OBJECT
    Q_PROPERTY(QString datasource READ datasourceName WRITE setDataSourceName )
public:
    DataBandDesignIntf(BandsType bandType, QString xmlTypeName, QObject* owner = 0, QGraphicsItem* parent=0);
};

class Segment{
public:
    Segment(qreal segmentStart,qreal segmentEnd):m_begin(segmentStart),m_end(segmentEnd){}
    bool intersect(Segment value);
    qreal intersectValue(Segment value);
private:
    qreal m_begin;
    qreal m_end;
};

class VSegment : public Segment{
public:
    VSegment(QRectF rect):Segment(rect.top(),rect.bottom()){}
};

struct HSegment :public Segment{
public:
    HSegment(QRectF rect):Segment(rect.left(),rect.right()){}
};

struct ItemSortContainer {
    QRectF m_rect;
    BaseDesignIntf * m_item;
    ItemSortContainer(BaseDesignIntf *item){
        m_item=item;
        m_rect=item->geometry();
    }
};

bool itemSortContainerLessThen(const PItemSortContainer c1, const PItemSortContainer c2);
bool bandIndexLessThen(const BandDesignIntf* b1, const BandDesignIntf* b2);

}
#endif // LRBANDDESIGNINTF_H
