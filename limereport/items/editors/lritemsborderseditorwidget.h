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
    explicit ItemsBordersEditorWidget(const QString &title, QWidget *parent = 0)
        : ItemEditorWidget(title, parent), m_changing(false), m_borders(0){
        initEditor();
    }
    bool changing() const;
protected slots:
    virtual void noBordesClicked();
    virtual void allBordesClicked();
    virtual void buttonClicked(bool);
    virtual void editBorderClicked();
protected:
    void setItemEvent(BaseDesignIntf *item);
    void properyChangedEvent(const QString &property, const QVariant &oldValue, const QVariant &newValue);
    BaseDesignIntf::BorderLines createBorders();
    BaseDesignIntf *m_item;
private:
    void initEditor();
    void updateValues(BaseDesignIntf::BorderLines borders);
    QAction* m_noLines;
    QAction* m_leftLine;
    QAction* m_rightLine;
    QAction* m_topLine;
    QAction* m_bottomLine;
    QAction* m_allLines;
    QAction* m_BorderEditor;
    bool m_changing;
    int m_borders;

};

#ifdef HAVE_REPORT_DESIGNER
class ItemsBordersEditorWidgetForDesigner : public ItemsBordersEditorWidget{
    Q_OBJECT
public:
    explicit ItemsBordersEditorWidgetForDesigner(ReportDesignWidget* reportEditor, const QString &title="", QWidget *parent = 0)
        : ItemsBordersEditorWidget(title,parent), m_reportEditor(reportEditor){}
protected slots:
    void buttonClicked(bool);
    void noBordesClicked();
    void allBordesClicked();
    void editBorderClicked();
private:
    ReportDesignWidget* m_reportEditor;

};
#endif

}//namespace LimeReport

#endif // LRITEMSBORDERSEDITORWIDGET_H
