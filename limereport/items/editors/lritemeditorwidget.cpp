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
#include "lritemeditorwidget.h"

namespace LimeReport{

void ItemEditorWidget::setItem(BaseDesignIntf* item)
{
    if (m_item!=item){
        if (m_item) m_item->disconnect(this);
        m_item=item;
        connect(m_item,SIGNAL(destroyed(QObject*)),this,SLOT(slotItemDestroyed(QObject*)));
        connect(m_item,SIGNAL(propertyChanged(QString,QVariant,QVariant)),
                this,SLOT(slotPropertyChanged(QString,QVariant,QVariant)));
        setEnabled(false);
        setItemEvent(item);
    }
}

void ItemEditorWidget::properyChangedEvent(const QString& propertName, const QVariant& oldValue, const QVariant& newValue)
{
    Q_UNUSED(propertName)
    Q_UNUSED(oldValue)
    Q_UNUSED(newValue)
}

void ItemEditorWidget::slotItemDestroyed(QObject* item)
{
    if (item==m_item) {
        m_item = 0;
        setEnabled(false);
    }
}

void ItemEditorWidget::slotPropertyChanged(const QString& propertName, const QVariant& oldValue, const QVariant& newValue)
{
    properyChangedEvent(propertName,oldValue,newValue);
}

}




