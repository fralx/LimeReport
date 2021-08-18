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
#ifndef LRITEMEDITORWIDGET_H
#define LRITEMEDITORWIDGET_H

#include <QToolBar>

#ifdef HAVE_REPORT_DESIGNER
#include "lrreportdesignwidget.h"
#endif
#include "lrpagedesignintf.h"

namespace LimeReport {

class ItemEditorWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit ItemEditorWidget(const QString &title, QWidget *parent = 0)
        : QToolBar(title, parent), m_item(0){}
    void setItem(BaseDesignIntf *item);
protected:
    virtual void setItemEvent(BaseDesignIntf*){}
    virtual void properyChangedEvent(const QString& propertName, const QVariant& oldValue, const QVariant& newValue);
    BaseDesignIntf* item(){return m_item;}
private slots:
    void slotItemDestroyed(QObject* item);
    void slotPropertyChanged(const QString& propertName, const QVariant& oldValue, const QVariant& newValue);
private:
    BaseDesignIntf* m_item;
};

} // namespace LimeReport
#endif // LRITEMEDITORWIDGET_H
