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
#ifndef LRPAGEITEM_H
#define LRPAGEITEM_H
#include "lrbasedesignintf.h"
#include "lrbanddesignintf.h"
#include "lritemscontainerdesignitf.h"
#include <QList>
#include <QColor>

namespace LimeReport{

class ReportRender;
class PageItemDesignIntf : public LimeReport::ItemsContainerDesignInft
{
    Q_OBJECT
    Q_ENUMS(Orientation)
    Q_ENUMS(PageSize)
    Q_PROPERTY(int topMargin READ topMargin WRITE setTopMargin)
    Q_PROPERTY(int bottomMargin READ bottomMargin WRITE setBottomMargin)
    Q_PROPERTY(int rightMargin READ rightMargin WRITE setRightMargin)
    Q_PROPERTY(int leftMargin READ leftMargin WRITE setLeftMargin)
    Q_PROPERTY(Orientation pageOrientation READ pageOrientation WRITE setPageOrientation)
    Q_PROPERTY(PageSize pageSize READ pageSize WRITE setPageSize )
    Q_PROPERTY(int gridStep READ gridStep WRITE setGridStep)
    Q_PROPERTY(bool fullPage READ fullPage WRITE setFullPage)
    Q_PROPERTY(bool oldPrintMode READ oldPrintMode WRITE setOldPrintMode)
    Q_PROPERTY(bool resetPageNumber READ resetPageNumber WRITE setResetPageNumber)
    friend class ReportRender;
public:
    enum Orientation { Portrait, Landscape };
    enum PageSize {A4, B5, Letter, Legal, Executive,
                   A0, A1, A2, A3, A5, A6, A7, A8, A9, B0, B1,
                   B10, B2, B3, B4, B6, B7, B8, B9, C5E, Comm10E,
                   DLE, Folio, Ledger, Tabloid, Custom, NPageSize = Custom
                  };
    typedef QList<BandDesignIntf*> BandsList;
    typedef QList<BandDesignIntf*>::const_iterator BandsIterator;
    typedef QSharedPointer<PageItemDesignIntf> Ptr;
    static PageItemDesignIntf::Ptr create(QObject* owner);
    explicit PageItemDesignIntf(QObject *owner = 0, QGraphicsItem* parent=0);
    explicit PageItemDesignIntf(const PageSize pageSize, const QRectF& rect,QObject *owner = 0, QGraphicsItem* parent=0);
    ~PageItemDesignIntf();
    virtual void paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
    virtual void geometryChangedEvent(QRectF newRect, QRectF);
    virtual QColor selectionColor() const;
    virtual QColor pageBorderColor() const;
    virtual QColor gridColor() const;
    void clear();
    const BandsList& childBands() const {return m_bands;}
    BandDesignIntf * bandByType(BandDesignIntf::BandsType bandType) const;
    bool isBandExists(BandDesignIntf::BandsType bandType);
    bool isBandExists(const QString& bandType);
    BandDesignIntf* bandByIndex(int index);
    BandDesignIntf* bandByName(QString bandObjectName);
    int  calcBandIndex(BandDesignIntf::BandsType bandType, BandDesignIntf* parentBand, bool &increaseBandIndex);
    void increaseBandIndex(int startIndex);
    bool isBandRegistred(BandDesignIntf* band);
    void registerBand(BandDesignIntf* band);
    void relocateBands();
    void removeBand(BandDesignIntf* band);

    int dataBandCount();
    BandDesignIntf* dataBandAt(int index);

    void setTopMargin(int value);
    int topMargin() const {return m_topMargin;}
    void setBottomMargin(int value);
    int bottomMargin() const {return m_bottomMargin;}
    void setLeftMargin(int value);
    int leftMargin() const {return m_leftMargin;}
    void setRightMargin(int value);
    int rightMargin() const{return m_rightMargin;}

    void setPageOrientation(Orientation value);
    Orientation pageOrientation() const {return m_pageOrientaion;}
    PageSize pageSize() const {return m_pageSize;}
    void setPageSize(const PageSize &size);
    QList<BandDesignIntf *>& bands();
    void setGridStep(int value);
    int gridStep();
    void objectLoadFinished();
    bool fullPage() const;
    void setFullPage(bool fullPage);

    bool oldPrintMode() const;
    void setOldPrintMode(bool oldPrintMode);
    bool canContainChildren(){ return true;}
    bool resetPageNumber() const;
    void setResetPageNumber(bool resetPageNumber);
    void updateSubItemsSize(RenderPass pass, DataSourceManager *dataManager);    
    void swapBands(BandDesignIntf *band, BandDesignIntf *bandToSwap);    
    void moveBandFromTo(int from, int to);

protected slots:
    void bandDeleted(QObject* band);
    void bandPositionChanged(QObject* object, QPointF newPos, QPointF oldPos);
protected:
    void    collectionLoadFinished(const QString& collectionName);
    QRectF& pageRect(){return m_pageRect;}
    void    updateMarginRect();
    QSizeF  getRectByPageSize(const PageSize &size);
    void    initPageSize(const PageSize &size);
    void    initPageSize(const QSizeF &size);
    QColor  selectionMarkerColor(){return Qt::transparent;}
    void    preparePopUpMenu(QMenu &menu);
private:
    void paintGrid(QPainter *ppainter);
    void initColumnsPos(QVector<qreal>&posByColumns, qreal pos, int columnCount);
private:
    int m_topMargin;
    int m_bottomMargin;
    int m_leftMargin;
    int m_rightMargin;
    Orientation m_pageOrientaion;
    PageSize m_pageSize;
    QRectF m_pageRect;
    bool m_sizeChainging;
    QList<BandDesignIntf*> m_bands;
    bool m_fullPage;
    bool m_oldPrintMode;
    bool m_resetPageNumber;
};

typedef QList<PageItemDesignIntf::Ptr> ReportPages;

}
#endif // LRPAGEITEM_H
