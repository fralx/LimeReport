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
#ifndef LRPAGEITEM_H
#define LRPAGEITEM_H
#include "lrbasedesignintf.h"
#include "lrbanddesignintf.h"
#include "lritemscontainerdesignitf.h"
#include <QList>
#include <QColor>
#include <QPrinter>

namespace LimeReport{

class ReportRender;
class LIMEREPORT_EXPORT PageItemDesignIntf : public ItemsContainerDesignInft
{
    Q_OBJECT
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
    Q_PROPERTY(bool isExtendedInDesignMode READ isExtendedInDesignMode WRITE setExtendedInDesignMode)
    Q_PROPERTY(int  extendedHeight READ extendedHeight WRITE setExtendedHeight)
    Q_PROPERTY(bool pageIsTOC READ isTOC WRITE setIsTOC)
    Q_PROPERTY(bool setPageSizeToPrinter READ getSetPageSizeToPrinter WRITE setSetPageSizeToPrinter )
    Q_PROPERTY(bool endlessHeight READ endlessHeight WRITE setEndlessHeight)
    Q_PROPERTY(bool printable READ isPrintable WRITE setPrintable)
    Q_PROPERTY(QString printerName READ printerName WRITE setPrinterName)
    Q_PROPERTY(UnitType units READ unitType WRITE setUnitTypeProperty)
    Q_PROPERTY(PrintBehavior printBehavior READ printBehavior WRITE setPrintBehavior)
    Q_PROPERTY(bool dropPrinterMargins READ dropPrinterMargins WRITE setDropPrinterMargins)
    Q_PROPERTY(bool notPrintIfEmpty READ notPrintIfEmpty WRITE setNotPrintIfEmpty)
    Q_PROPERTY(bool mixWithPriorPage READ mixWithPriorPage WRITE setMixWithPriorPage)
    friend class ReportRender;
public:
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    enum Orientation { Portrait = QPrinter::Portrait, Landscape = QPrinter::Landscape };

    enum PrintBehavior {Scale, Split};

    enum PageSize {
        A4 = QPrinter::A4, B5 = QPrinter::B5, Letter = QPrinter::Letter,
        Legal = QPrinter::Legal, Executive = QPrinter::Executive,
        A0 = QPrinter::A0, A1 = QPrinter::A1, A2 = QPrinter::A2, A3 = QPrinter::A3,
        A5 = QPrinter::A5, A6 = QPrinter::A6, A7 = QPrinter::A7, A8 = QPrinter::A8,
        A9 = QPrinter::A9, B0 = QPrinter::B0, B1 = QPrinter::B1, B10 = QPrinter::B10,
        B2 = QPrinter::B2, B3 = QPrinter::B3, B4 = QPrinter::B4, B6 = QPrinter::B6,
        B7 = QPrinter::B7, B8 = QPrinter::B8, B9 = QPrinter::B9, C5E = QPrinter::C5E,
        Comm10E = QPrinter::Comm10E, DLE = QPrinter::DLE, Folio = QPrinter::Folio,
        Ledger = QPrinter::Ledger, Tabloid = QPrinter::Tabloid, Custom = QPrinter::Custom,
        NPageSize = Custom
    };
#else
    enum Orientation { Portrait = QPageLayout::Portrait, Landscape = QPageLayout::Landscape };

    enum PrintBehavior {Scale, Split};

    enum PageSize {
        A4 = QPageSize::A4, B5 = QPageSize::B5, Letter = QPageSize::Letter,
        Legal = QPageSize::Legal, Executive = QPageSize::Executive,
        A0 = QPageSize::A0, A1 = QPageSize::A1, A2 = QPageSize::A2, A3 = QPageSize::A3,
        A5 = QPageSize::A5, A6 = QPageSize::A6, A7 = QPageSize::A7, A8 = QPageSize::A8,
        A9 = QPageSize::A9, B0 = QPageSize::B0, B1 = QPageSize::B1, B10 = QPageSize::B10,
        B2 = QPageSize::B2, B3 = QPageSize::B3, B4 = QPageSize::B4, B6 = QPageSize::B6,
        B7 = QPageSize::B7, B8 = QPageSize::B8, B9 = QPageSize::B9, C5E = QPageSize::C5E,
        Comm10E = QPageSize::Comm10E, DLE = QPageSize::DLE, Folio = QPageSize::Folio,
        Ledger = QPageSize::Ledger, Tabloid = QPageSize::Tabloid, Custom = QPageSize::Custom,
        NPageSize = Custom
    };
#endif

#if QT_VERSION >= 0x050500
    Q_ENUM(Orientation)
    Q_ENUM(PrintBehavior)
    Q_ENUM(PageSize)
#else
    Q_ENUMS(Orientation)
    Q_ENUMS(PrintBehavior)
    Q_ENUMS(PageSize)
#endif
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
    virtual QRectF boundingRect() const;
    void setItemMode(LimeReport::BaseDesignIntf::ItemMode mode);
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
    bool canContainChildren() const{ return true;}
    bool canAcceptPaste() const{ return true;}
    bool resetPageNumber() const;
    void setResetPageNumber(bool resetPageNumber);
    void updateSubItemsSize(RenderPass pass, DataSourceManager *dataManager);    
    void swapBands(BandDesignIntf *band, BandDesignIntf *bandToSwap);    
    void moveBandFromTo(int from, int to);

    QList<BandDesignIntf *> createBandGroup(int beginIndex, int endIndex);
    
    bool isExtendedInDesignMode() const;
    void setExtendedInDesignMode(bool isExtendedInDesignMode);
    int  extendedHeight() const;
    void setExtendedHeight(int extendedHeight);

    bool isTOC() const;
    void setIsTOC(bool isTOC);
    bool getSetPageSizeToPrinter() const;
    void setSetPageSizeToPrinter(bool setPageSizeToPrinter);

    bool endlessHeight() const;
    void setEndlessHeight(bool endlessHeight);

    bool isPrintable() const;
    void setPrintable(bool printable);

    QString printerName() const;
    void setPrinterName(const QString& printerName);

    void placeTearOffBand();
    BandDesignIntf *pageFooter() const;
    void setPageFooter(BandDesignIntf *pageFooter);
    
    PrintBehavior printBehavior() const;
    void setPrintBehavior(const PrintBehavior &printBehavior);

    bool dropPrinterMargins() const;
    void setDropPrinterMargins(bool dropPrinterMargins);

    bool isEmpty() const;

    bool notPrintIfEmpty() const;
    void setNotPrintIfEmpty(bool notPrintIfEmpty);

    bool mixWithPriorPage() const;
    void setMixWithPriorPage(bool value);

signals:
    void beforeFirstPageRendered();
    void afterLastPageRendered();
protected slots:
    void bandDeleted(QObject* band);
    void bandPositionChanged(QObject* object, QPointF newPos, QPointF oldPos);
    void bandGeometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry);
    void setUnitTypeProperty(BaseDesignIntf::UnitType value);
protected:
    void    collectionLoadFinished(const QString& collectionName);
    QRectF& pageRect(){return m_pageRect;}
    void    updateMarginRect();
    QSizeF  getRectByPageSize(const PageSize &size);
    void    initPageSize(const PageSize &size);
    void    initPageSize(const QSizeF &size);
    QColor  selectionMarkerColor(){return Qt::transparent;}
    void    preparePopUpMenu(QMenu &menu);
    void    processPopUpAction(QAction *action);
private:
    void paintGrid(QPainter *ppainter, QRectF rect);
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
    bool m_isExtendedInDesignMode;
    int  m_extendedHeight;
    bool m_isTOC;
    bool m_setPageSizeToPrinter;
    bool m_endlessHeight;
    bool m_printable;
    QString m_printerName;
    BandDesignIntf* m_pageFooter;
    PrintBehavior m_printBehavior;
    bool m_dropPrinterMargins;
    bool m_notPrintIfEmpty;
    bool m_mixWithPriorPage;


};

typedef QList<PageItemDesignIntf::Ptr> ReportPages;

}
#endif // LRPAGEITEM_H
