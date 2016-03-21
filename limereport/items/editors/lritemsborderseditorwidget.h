/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2015 by Alexander Arin                                  *
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
#ifndef LRITEMSBORDERSEDITORWIDGET_H
#define LRITEMSBORDERSEDITORWIDGET_H

#include <QToolBar>
#include "lrreportdesignwidget.h"
#include "lritemeditorwidget.h"

namespace LimeReport{

class ItemsBordersEditorWidget : public ItemEditorWidget
{
    Q_OBJECT
public:
    explicit ItemsBordersEditorWidget(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0);
    explicit ItemsBordersEditorWidget(ReportDesignWidget* reportEditor, QWidget *parent = 0);
private slots:
    void noBordesClicked();
    void allBordesClicked();
    void buttonClicked(bool);
protected:
    void setItemEvent(BaseDesignIntf *item);
    void properyChangedEvent(const QString &property, const QVariant &oldValue, const QVariant &newValue);
private:
    void initEditor();
    void updateValues(BaseDesignIntf::BorderLines borders);
    BaseDesignIntf::BorderLines createBorders();
    QAction* m_noLines;
    QAction* m_leftLine;
    QAction* m_rightLine;
    QAction* m_topLine;
    QAction* m_bottomLine;
    QAction* m_allLines;
    bool m_changing;
};

}//namespace LimeReport

#endif // LRITEMSBORDERSEDITORWIDGET_H
