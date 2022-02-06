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
#ifndef LRITEMDESIGNITTF_H
#define LRITEMDESIGNITTF_H

#include <QObject>
#include "lrbasedesignintf.h"

namespace LimeReport{
class BaseDesignIntf;
class ItemDesignIntf : public BaseDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(LocationType itemLocation READ itemLocation WRITE setItemLocation)
    Q_PROPERTY(bool stretchToMaxHeight READ stretchToMaxHeight WRITE setStretchToMaxHeight)
    Q_PROPERTY(ItemAlign itemAlign READ itemAlign WRITE setItemAlign)
public:
    enum LocationType{Band,Page};
#if QT_VERSION >= 0x050500
    Q_ENUM(LocationType)
#else
    Q_ENUMS(LocationType)
#endif
    ItemDesignIntf(const QString& xmlTypeName, QObject* owner = 0,QGraphicsItem* parent = 0);
    LocationType itemLocation(){return m_itemLocation;}
    void setItemLocation(LocationType location);
    void setStretchToMaxHeight(bool value);
    bool stretchToMaxHeight(){return m_stretchToMaxHeight;}
    BaseDesignIntf* cloneEmpty(int height, QObject *owner, QGraphicsItem *parent);
signals:
    void itemLocationChanged(BaseDesignIntf* item, BaseDesignIntf* parent);
protected:
    QGraphicsItem* bandByPos();
    virtual void initFlags();
private:
    LocationType m_itemLocation;
    bool m_stretchToMaxHeight;
};

class Spacer :public ItemDesignIntf{
public:
    Spacer(QObject* owner,QGraphicsItem* parent);
    bool isEmpty() const {return true;}
protected:
    BaseDesignIntf* createSameTypeItem(QObject *owner, QGraphicsItem *parent){
        return new Spacer(owner, parent);
    }
};

class ContentItemDesignIntf : public ItemDesignIntf
{
    Q_OBJECT
public:
    ContentItemDesignIntf(const QString& xmlTypeName, QObject* owner = 0,QGraphicsItem* parent = 0)
        :ItemDesignIntf(xmlTypeName,owner,parent), m_contentBackedUp(false){}
    virtual QString content() const = 0;
    virtual void setContent(const QString& value) = 0;
    QMap<QString, QString> getStringForTranslation();
    void backupContent(){ m_contentBackUp = content(); m_contentBackedUp = true;}
    void restoreContent() {setContent(m_contentBackUp);}
    bool isContentBackedUp() const;
    void setContentBackedUp(bool contentBackedUp);
private:
    QString m_contentBackUp;
    bool m_contentBackedUp;
};

class LayoutDesignIntf : public ItemDesignIntf{
public:
    LayoutDesignIntf(const QString& xmlTypeName, QObject* owner = 0,QGraphicsItem* parent = 0):
        ItemDesignIntf(xmlTypeName,owner,parent){}
    virtual void addChild(BaseDesignIntf *item,bool updateSize=true) = 0;
    virtual void removeChild(BaseDesignIntf *item) = 0;
    virtual void restoreChild(BaseDesignIntf *item) = 0;
    virtual int childrenCount() = 0;
    friend class BaseDesignIntf;
};

}
#endif // LRITEMDESIGNITTF_H
