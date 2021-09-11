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
#ifndef LRSUBDETAILBAND_H
#define LRSUBDETAILBAND_H

#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"

namespace LimeReport{

class SubDetailBand : public DataBandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool keepFooterTogether READ keepFooterTogether WRITE setKeepFooterTogether)
    Q_PROPERTY(QColor alternateBackgroundColor READ alternateBackgroundColor WRITE setAlternateBackgroundColor)
    Q_PROPERTY(bool useAlternateBackgroundColor READ useAlternateBackgroundColor WRITE setUseAlternateBackgroundColor)
public:
    SubDetailBand(QObject* owner = 0, QGraphicsItem* parent=0);
    bool isUnique() const {return false;}
    int  bandNestingLevel(){ return 1;}
    bool isHasHeader() const;
    bool isHasFooter() const;
private:
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
protected:
    virtual QColor bandColor() const;
};

class SubDetailHeaderBand : public BandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool printAlways READ printAlways WRITE setPrintAlways)
public:
    SubDetailHeaderBand(QObject* owner = 0, QGraphicsItem* parent=0);
    bool isUnique() const;
    bool isHeader() const {return true;}
    int  bandNestingLevel(){ return 1;}
protected:
    QColor bandColor() const;
private:
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
};

class SubDetailFooterBand : public BandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool printAlways READ printAlways WRITE setPrintAlways)
public:
    SubDetailFooterBand(QObject* owner = 0, QGraphicsItem* parent=0);
    virtual bool isUnique() const;
    bool isFooter() const{return true;}
    int  bandNestingLevel(){ return 1;}
protected:
    QColor bandColor() const;
private:
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
private:
};

}
#endif // LRSUBDETAILBAND_H
