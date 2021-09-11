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
#ifndef LRDATABAND_H
#define LRDATABAND_H

#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"
#include <QObject>

namespace LimeReport{

class DataBand : public DataBandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(bool keepSubdetailTogether READ tryToKeepTogether WRITE setTryToKeepTogether)
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
    Q_PROPERTY(bool keepFooterTogether READ keepFooterTogether WRITE setKeepFooterTogether)
    Q_PROPERTY(bool sliceLastRow READ sliceLastRow WRITE setSliceLastRow)
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool startNewPage READ startNewPage WRITE setStartNewPage)
    Q_PROPERTY(bool startFromNewPage READ startFromNewPage WRITE setStartFromNewPage)
    Q_PROPERTY(QColor alternateBackgroundColor READ alternateBackgroundColor WRITE setAlternateBackgroundColor)
    Q_PROPERTY(bool useAlternateBackgroundColor READ useAlternateBackgroundColor WRITE setUseAlternateBackgroundColor)
public:
    DataBand(QObject* owner = 0, QGraphicsItem* parent=0);
    bool isUnique() const;
    bool isData() const {return true;}
protected:
    QColor bandColor() const;
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
private:
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
};

class DataHeaderBand : public BandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(bool reprintOnEachPage READ reprintOnEachPage WRITE setReprintOnEachPage)
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool printAlways READ printAlways WRITE setPrintAlways)
    Q_PROPERTY(bool repeatOnEachRow READ repeatOnEachRow WRITE setRepeatOnEachRow)
public:
    DataHeaderBand(QObject* owner=0, QGraphicsItem* parent=0);
    bool isUnique() const {return false;}
    bool isHeader() const {return true;}
    QColor bandColor() const {return QColor(Qt::darkGreen);}
protected:
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
private:
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0){
        return new DataHeaderBand(owner,parent);
    }
};

class DataFooterBand : public BandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(int columnsCount READ columnsCount WRITE setColumnsCount)
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
    Q_PROPERTY(BandColumnsLayoutType columnsFillDirection READ columnsFillDirection WRITE setColumnsFillDirection)
    Q_PROPERTY(bool printAlways READ printAlways WRITE setPrintAlways)
public:
    DataFooterBand(QObject* owner=0, QGraphicsItem* parent=0);
    bool isUnique() const {return false;}
    bool isFooter() const {return true;}
    QColor bandColor() const{return QColor(Qt::darkGreen);}
protected:
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
private:
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0){
        return new DataFooterBand(owner,parent);
    }
};

}
#endif // LRDATABAND_H
