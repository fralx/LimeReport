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
#include "lrdatabrowsertree.h"
#include <QDebug>
#include <QMimeData>

namespace LimeReport {

DataBrowserTree::DataBrowserTree(QWidget *parent) :
    QTreeWidget(parent){}

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
QMimeData *DataBrowserTree::mimeData(const QList<QTreeWidgetItem *> &items) const
#else
QMimeData *DataBrowserTree::mimeData(const QList<QTreeWidgetItem *> items) const
#endif
{
    QMimeData* result = QTreeWidget::mimeData(items);
    if (items.at(0)->type()==Row){
        result->setText("field:$D{"+items.at(0)->parent()->text(0)+"."+items.at(0)->data(0,Qt::DisplayRole).toString()+"}");
    }
    if (items.at(0)->type()==Variable){
        result->setText("variable:$V{"+items.at(0)->text(0)+"}");
    }
    if (items.at(0)->type()==ExternalVariable){
        result->setText("variable:$V{"+items.at(0)->text(0)+"}");
    }
    return result;
}

} // namespace LimeReport
