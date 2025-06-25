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
#ifndef LRREPORTFOOTER_H
#define LRREPORTFOOTER_H

#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"

#include <QObject>

namespace LimeReport {
class ReportFooter: public BandDesignIntf {
    Q_OBJECT
    Q_PROPERTY(int maxScalePercent READ maxScalePercent WRITE setMaxScalePercent)
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable)
public:
    ReportFooter(QObject* owner = 0, QGraphicsItem* parent = 0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner = 0, QGraphicsItem* parent = 0);

protected:
    QColor bandColor() const;
    bool isFooter() const;
};
} // namespace LimeReport
#endif // LRREPORTFOOTER_H
