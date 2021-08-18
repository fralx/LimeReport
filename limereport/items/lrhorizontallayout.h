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
#ifndef LRHORIZONTALLAYOUT_H
#define LRHORIZONTALLAYOUT_H
#include "lritemdesignintf.h"
#include "lrlayoutmarker.h"
#include "lrabstractlayout.h"

namespace LimeReport
{

class HorizontalLayout : public AbstractLayout
{
    Q_OBJECT
    Q_PROPERTY(LayoutType layoutType READ layoutType WRITE setLayoutType)
public:
    friend class LayoutMarker;
    friend class BaseDesignIntf;

    HorizontalLayout(QObject *owner = 0, QGraphicsItem *parent = 0);
    ~HorizontalLayout();
    BaseDesignIntf *createSameTypeItem(QObject *owner = 0, QGraphicsItem *parent = 0);
    bool isSplittable() const { return true;}
    bool canContainChildren() const { return true;}

protected:
    void updateLayoutSize();
    void relocateChildren();
    bool canBeSplitted(int height) const;
    BaseDesignIntf* cloneUpperPart(int height, QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner=0, QGraphicsItem *parent=0);
    void setItemAlign(const ItemAlign &itemAlign);

private:
    void sortChildren();
    void divideSpace();
    void placeItemInLayout(BaseDesignIntf* item);
    void insertItemInLayout(BaseDesignIntf* item);
};

} //namespace LimeReport
#endif // LRHORIZONTALLAYOUT_H
