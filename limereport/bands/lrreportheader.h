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
#ifndef LRREPORTHEADER_H
#define LRREPORTHEADER_H

#include <QObject>
#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"

namespace LimeReport{
class ReportHeader : public LimeReport::BandDesignIntf
{
    Q_OBJECT
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
    Q_PROPERTY(bool printBeforePageHeader READ printBeforePageHeader WRITE setPrintBeforePageHeader)
public:
    ReportHeader(QObject* owner = 0, QGraphicsItem *parent=0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
    bool printBeforePageHeader() const;
    void setPrintBeforePageHeader(bool printBeforePageHeader);
    bool isHeader() const {return true;}
protected:
    QColor bandColor() const;
    bool m_printBeforePageHeader;
};
}
#endif // LRREPORTHEADER_H
