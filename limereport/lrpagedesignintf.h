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
#ifndef LRPAGEDEDIGNITF_H
#define LRPAGEDEDIGNITF_H

#include <QGraphicsScene>
#include <QtXml>

#include "lrpageitemdesignintf.h"
#include "serializators/lrstorageintf.h"
#include "lrbanddesignintf.h"

namespace LimeReport {

    class BaseDesignIntf;
    class ReportEnginePrivate;
    class PropertyChangedCommand;
    class HorizontalLayout;
    class VerticalLayout;
    class LayoutDesignIntf;

    class CommandIf {
    public:
        virtual ~CommandIf(){}
        typedef QSharedPointer<CommandIf> Ptr;
        virtual bool doIt() = 0;
        virtual void undoIt() = 0;
        virtual void addCommand(CommandIf::Ptr command,bool execute);
    };

    typedef QList<LimeReport::BandDesignIntf*>::const_iterator BandsIterator;

    struct ReportItemPos{
        QString objectName;
        QPointF pos;
    };

    struct ReportItemSize{
        QString objectName;
        QSizeF  size;
    };

    class Projection{
    public:
        Projection(qreal start, qreal end)
            :m_start(start),m_end(end){}
        bool intersect(Projection projection);
        qreal start() const;
        qreal end() const;
    private:
        qreal m_start;
        qreal m_end;
    };

    class ItemProjections{
    public:
        ItemProjections(BaseDesignIntf* item)
            :m_xProjection(item->pos().x(), item->pos().x()+item->width()),
            m_yProjection(item->pos().y(), item->pos().y()+item->height()),
            m_item(item)
        {}
        bool intersect(QRectF rect);
        bool intersect(BaseDesignIntf* item);
        qreal square(QRectF rect);
        qreal square(BaseDesignIntf* item);
        BaseDesignIntf* item(){return m_item;}
    private:
        Projection m_xProjection;
        Projection m_yProjection;
        BaseDesignIntf* m_item;
    };

    class PageDesignIntf : public QGraphicsScene, public ObjectLoadingStateIntf{
        Q_OBJECT
        Q_PROPERTY(QObject* pageItem READ pageItem)
    public:
        friend class PropertyChangedCommand;
        friend class InsertHLayoutCommand;
        friend class InsertVLayoutCommand;
        explicit PageDesignIntf(QObject* parent = 0);
        ~PageDesignIntf();
        void updatePageRect();
        void startInsertMode(const QString& ItemType);
        void startEditMode();

        PageItemDesignIntf *pageItem();
        void setPageItem(PageItemDesignIntf::Ptr pageItem);
        void setPageItems(QList<PageItemDesignIntf::Ptr> pages);
        void removePageItem(PageItemDesignIntf::Ptr pageItem);
        QList<PageItemDesignIntf::Ptr> pageItems(){return m_reportPages;}

        bool isItemInsertMode();
        ReportEnginePrivate* reportEditor();
        void setReportEditor(ReportEnginePrivate* value){m_reportEditor=value;}

        QStringList possibleParentItems();
        void registerItem(BaseDesignIntf* item);
        void registerBand(BandDesignIntf* band);
        void removeAllItems();

        void setItemMode(BaseDesignIntf::ItemMode state);
        BaseDesignIntf::ItemMode itemMode(){return m_itemMode;}
        BaseDesignIntf* reportItemByName(const QString& name);
        QList<BaseDesignIntf *> reportItemsByName(const QString &name);
        BandDesignIntf* bandAt(QPointF pos);
        BaseDesignIntf* addReportItem(const QString& itemType, QPointF pos, QSizeF size);
        BaseDesignIntf* addReportItem(const QString& itemType, QObject *owner=0, BaseDesignIntf *parent=0);
        BaseDesignIntf* createReportItem(const QString& itemType, QObject *owner=0, BaseDesignIntf *parent=0);
        void removeReportItem(BaseDesignIntf* item, bool createComand = true);
        CommandIf::Ptr removeReportItemCommand(BaseDesignIntf *item);
        bool saveCommand(CommandIf::Ptr command, bool runCommand = true);

        bool isCanRedo();
        bool isCanUndo();
        bool isHasChanges();

        void reactivatePageItem(PageItemDesignIntf::Ptr pageItem);

        void setSettings(QSettings* settings){ m_settings = settings;}
        QSettings* settings(){ return m_settings;}

        QString genObjectName(const QObject& object);

        void animateItem(BaseDesignIntf* item);
        void setSelectionRect(QRectF selectionRect);
        void emitRegisterdItem(BaseDesignIntf *item);
        void emitItemRemoved(BaseDesignIntf* item);

        DataSourceManager* datasourceManager();
        bool isSaved(){ return !m_hasChanges;}
        void changeSelectedGrpoupTextAlignPropperty(const bool& horizontalAlign, Qt::AlignmentFlag flag);

        int verticalGridStep() const;
        void setVerticalGridStep(int verticalGridStep);

        int horizontalGridStep() const;
        void setHorizontalGridStep(int horizontalGridStep);

        void beginUpdate(){m_updating = true;}
        bool isUpdating(){return m_updating;}
        void endUpdate();

        void rectMoved(QRectF itemRect, BaseDesignIntf* container = 0);
        void itemMoved(BaseDesignIntf* item);
        bool magneticMovement() const;
        void setMagneticMovement(bool magneticMovement);

        ReportSettings *getReportSettings() const;
        void setReportSettings(ReportSettings *reportSettings);

        void setPropertyToSelectedItems(const char *name, const QVariant &value);

        PageItemDesignIntf* getCurrentPage() const;
        void setCurrentPage(PageItemDesignIntf* currentPage);

    protected:

        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent *event);
        virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
        virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

        virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
        virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *);
        virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
        virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

        LimeReport::BandDesignIntf::BandsType findPriorType(LimeReport::BandDesignIntf::BandsType bandType);
        BaseDesignIntf *findDestObject(BaseDesignIntf *item);

        bool isExistsObjectName (const QString& objectName, QList<QGraphicsItem *> &itemsList) const;

        bool isLoading();
        void objectLoadStarted();
        void objectLoadFinished();

        HorizontalLayout* internalAddHLayout();
        VerticalLayout* internalAddVLayout();
        QPointF placePosOnGrid(QPointF point);
        QSizeF placeSizeOnGrid(QSizeF size);
    signals:
        void geometryChanged(QRectF newGeometry);
        void insertModeStarted();
        void itemInserted(LimeReport::PageDesignIntf* report, QPointF pos, const QString& ItemType);
        void itemInsertCanceled(const QString& ItemType);
        void itemSelected(LimeReport::BaseDesignIntf *item);
        void multiItemsSelected(QList<QObject*>* objectsList);
        void miltiItemsSelectionFinished();
        void commandHistoryChanged();
        void itemPropertyChanged(const QString& objectName,
                                 const QString& propertyName,
                                 const QVariant& oldValue,
                                 const QVariant& newValue);
        void itemPropertyObjectNameChanged(const QString& oldName, const QString& newName);
        void itemAdded(LimeReport::PageDesignIntf* page, LimeReport::BaseDesignIntf* item);
        void itemRemoved(LimeReport::PageDesignIntf* page, LimeReport::BaseDesignIntf* item);
        void bandAdded(LimeReport::PageDesignIntf* page, LimeReport::BandDesignIntf* band);
        void bandRemoved(LimeReport::PageDesignIntf* page, LimeReport::BandDesignIntf* band);
        void pageUpdateFinished(LimeReport::PageDesignIntf* page);
    public slots:
        BaseDesignIntf* addBand(const QString& bandType);
        BaseDesignIntf* addBand(BandDesignIntf::BandsType bandType);
        void removeBand(LimeReport::BandDesignIntf* band);
        void bandGeometryChanged(QObject* object, QRectF newGeometry, QRectF oldGeometry);
        void bandPosChanged(QObject* object, QPointF newPos, QPointF oldPos);
        void slotUpdateItemSize();
        void undo();
        void redo();
        void copy();
        void paste();
        void deleteSelected();
        void cut();
        void setToSaved();
        void bringToFront();
        void sendToBack();
        void alignToLeft();
        void alignToRigth();
        void alignToVCenter();
        void alignToTop();
        void alignToBottom();
        void alignToHCenter();
        void sameWidth();
        void sameHeight();
        void addHLayout();
        void addVLayout();
        void setFont(const QFont &font);
        void setTextAlign(const Qt::Alignment& alignment);
        void setBorders(const BaseDesignIntf::BorderLines& border);
        void setBordersExt(const BaseDesignIntf::BorderLines &border,
                const double borderWidth,
                const BaseDesignIntf::BorderStyle style,
                const QString color
        );
        void lockSelectedItems();
        void unlockSelectedItems();
        void selectOneLevelItems();
    private slots:
        void slotPageGeometryChanged(QObject*, QRectF, QRectF );
        void slotItemPropertyChanged(QString propertyName,
                                     const QVariant &oldValue,
                                     const QVariant &newValue);
        void slotItemPropertyObjectNameChanged(const QString& oldName, const QString& newName);
        void bandDeleted(QObject* band);
        void slotPageItemLoaded(QObject *);
        void slotSelectionChanged();
        void slotAnimationStoped(QObject *animation);    
    private:
        template <typename T>
        BaseDesignIntf* internalAddBand(T bandType);
        void finalizeInsertMode();
        void saveSelectedItemsPos();
        void saveSelectedItemsGeometry();
        void checkSizeOrPosChanges();
        CommandIf::Ptr createChangePosCommand();
        CommandIf::Ptr createChangeSizeCommand();
        void saveChangeProppertyCommand(const QString& objectName,
                                        const QString& propertyName,
                                        const QVariant& oldPropertyValue,
                                        const QVariant& newPropertyValue);
        void changeSelectedGroupProperty(const QString& name,const QVariant& value);
        void activateItemToJoin(QRectF itemRect, QList<ItemProjections>& items);
        void selectAllChildren(BaseDesignIntf* item);
        bool selectionContainsBand();
    private:
        enum JoinType{Width, Height};
        LimeReport::PageItemDesignIntf::Ptr m_pageItem;
        QList<PageItemDesignIntf::Ptr> m_reportPages;
        ReportEnginePrivate* m_reportEditor;
        bool m_insertMode;
        QGraphicsItem * m_itemInsertRect;
        QString m_insertItemType;
        BaseDesignIntf::ItemMode m_itemMode;
        QGraphicsRectItem* m_cutterBorder;
        QGraphicsRectItem* m_pageRect;
//        QGraphicsTextItem* m_infoPosRect;
        QVector<CommandIf::Ptr> m_commandsList;
        QVector<ReportItemPos> m_positionStamp;
        QVector<ReportItemSize> m_geometryStamp;
        BaseDesignIntf* m_firstSelectedItem;
        int m_currentCommand;
        bool m_changeSizeMode;
        bool m_changePosMode;
        bool m_changePosOrSizeMode;
        bool m_executingCommand;
        bool m_hasChanges;
        bool m_isLoading;
        bool m_executingGroupCommand;
        QSettings* m_settings;
        QList<QObject*> m_animationList;
        QPointF m_startSelectionPoint;
        QGraphicsRectItem* m_selectionRect;
        int m_verticalGridStep;
        int m_horizontalGridStep;
        bool m_updating;
        int m_currentObjectIndex;
        bool m_multiSelectStarted;
        QList<ItemProjections> m_projections;
        BaseDesignIntf*  m_movedItem;
        BaseDesignIntf*  m_movedItemContainer;
        BaseDesignIntf*  m_joinItem;
        JoinType         m_joinType;
        bool             m_magneticMovement;
        ReportSettings*  m_reportSettings;
        PageItemDesignIntf* m_currentPage;

    };

    class AbstractPageCommand : public CommandIf{
    public:
        void setPage(PageDesignIntf* value){m_page = value;}
        PageDesignIntf* page(){return m_page;}
    private:
       PageDesignIntf* m_page;
    };

    class InsertHLayoutCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page);
        bool doIt();
        void undoIt();
    private:
        InsertHLayoutCommand(){}
    private:
        QString m_layoutName;
        QString m_oldParentName;
        QMap<QString,QPointF> m_elements;
    };

    class InsertVLayoutCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page);
        bool doIt();
        void undoIt();
    private:
        InsertVLayoutCommand(){}
    private:
        QString m_layoutName;
        QString m_oldParentName;
        QMap<QString,QPointF> m_elements;
    };

    class InsertItemCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString& itemType, QPointF pos, QSizeF size);
        bool doIt();
        void undoIt();
        void setPos(QPointF& value){m_pos = value;}
        QPointF pos(){return m_pos;}
        void setSize(QSizeF& value){m_size=value;}
        QSizeF size(){return m_size;}
        void setType(const QString& value){m_itemType=value;}
    private:
        InsertItemCommand(){}
    private:
        QPointF m_pos;
        QSizeF m_size;
        QString m_itemType;
        QString m_itemName;
    };

    class InsertBandCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page,const QString& bandName);
        bool doIt();
        void undoIt();
    private:
        BandDesignIntf::BandsType m_bandType;
        QString m_bandName;
        QString m_parentBandName;
    };

    class DeleteItemCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, BaseDesignIntf* item);
        bool doIt();
        void undoIt();
        void setType(const QString& value){m_itemType=value;}
        void setXml(const QString& value){m_itemXML=value;}
        void setItem(BaseDesignIntf* value);
    private:
        QString m_itemXML;
        QString m_itemType;
        QString m_itemName;
        QString m_layoutName;
    };

    class DeleteLayoutCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, LayoutDesignIntf* item);
        bool doIt();
        void undoIt();
    protected:
        void setItem(BaseDesignIntf* item);
    private:
        QStringList m_childItems;
        QString m_layoutName;
        QString m_itemXML;
        QString m_itemType;
        QString m_itemName;
    };


    class PasteCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString& itemsXML, BaseDesignIntf *parent);
        bool doIt();
        void undoIt();
    protected:
        void setItemsXML(const QString& itemsXML);
        void setParent(BaseDesignIntf* parent){m_parentItemName = parent->objectName();}
        bool insertItem(ItemsReaderIntf::Ptr reader);
        void changeName(PageDesignIntf* page, BaseDesignIntf *item);
    private:
        QString m_itemsXML;
        QString m_parentItemName;
        QVector<QString> m_itemNames;
    };

    class CutCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page);
        bool doIt();
        void undoIt();
        void setXML(const QString& value){m_itemsXML=value;}
    private:
        QString m_itemsXML;
        QVector<QString> m_itemNames;
    };

    class PosChangedCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, QVector<ReportItemPos>& oldPos, QVector<ReportItemPos>& newPos);
        bool doIt();
        void undoIt();
    private:
        QVector<ReportItemPos> m_oldPos;
        QVector<ReportItemPos> m_newPos;
    };

    class BandSwapCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString& bandName, const QString& bandToSwapName);
        bool doIt();
        void undoIt();
    private:
        QString bandName;
        QString bandToSwapName;
    };

    class BandMoveFromToCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, int from, int to);
        bool doIt();
        void undoIt();
    private:
        int from;
        int to;
        int reverceFrom;
        int reverceTo;
    };


    class SizeChangedCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, QVector<ReportItemSize>& oldSize, QVector<ReportItemSize>& newSize);
        bool doIt();
        void undoIt();
    private:
        QVector<ReportItemSize> m_oldSize;
        QVector<ReportItemSize> m_newSize;
    };

    class PropertyChangedCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString& objectName, const QString& propertyName, const QVariant& oldValue, const QVariant& newValue);
        bool doIt();
        void undoIt();
    private:
        QString     m_objectName;
        QString     m_propertyName;
        QVariant    m_oldValue;
        QVariant    m_newValue;
    };

    class PropertyObjectNameChangedCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString &oldValue, const QString &newValue);
        bool doIt();
        void undoIt();
    private:
        QString m_oldName;
        QString m_newName;
    };

    class PropertyItemAlignChangedCommand : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create(PageDesignIntf* page, const QString& objectName, BaseDesignIntf::ItemAlign oldValue, BaseDesignIntf::ItemAlign newValue);
        bool doIt();
        void undoIt();
    private:
        QString m_objectName;
        QString m_propertyName;
        BaseDesignIntf::ItemAlign m_oldValue;
        BaseDesignIntf::ItemAlign m_newValue;
        QPointF m_savedPos;
    };

    class CommandGroup : public AbstractPageCommand{
    public:
        static CommandIf::Ptr create();
        bool doIt();
        void undoIt();
        void addCommand(CommandIf::Ptr command,bool execute);
    private:
        QList<CommandIf::Ptr> m_commands;
    };

}
#endif //LRPAGEDEDIGNITF_H
