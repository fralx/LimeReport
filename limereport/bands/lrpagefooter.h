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
#ifndef LRPAGEFOOTER_H
#define LRPAGEFOOTER_H

#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"

#include <QObject>

namespace LimeReport {
class PageFooter: public BandDesignIntf {
    Q_OBJECT
    Q_PROPERTY(bool printOnFirstPage READ printOnFirstPage WRITE setPrintOnFirstPage)
    Q_PROPERTY(bool printOnLastPage READ printOnLastPage WRITE setPrintOnLastPage)
    Q_PROPERTY(bool removeGap READ removeGap WRITE setRemoveGap)
public:
    PageFooter(QObject* owner = 0, QGraphicsItem* parent = 0);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner = 0, QGraphicsItem* parent = 0);
    virtual bool isFooter() const { return true; }
    bool printOnLastPage() const;
    void setPrintOnLastPage(bool printOnLastPage);
    bool printOnFirstPage() const;
    void setPrintOnFirstPage(bool printOnFirstPage);
    bool removeGap() const;
    void setRemoveGap(bool removeGap);

protected:
    QColor bandColor() const;
    void preparePopUpMenu(QMenu& menu);
    void processPopUpAction(QAction* action);

private:
    bool m_printOnFirstPage;
    bool m_printOnLastPage;
    bool m_removeGap;
};
} // namespace LimeReport

#endif // LRPAGEFOOTER_H
