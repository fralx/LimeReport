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
#ifndef LRTEXTALIGNMENTEDITORWIDGET_H
#define LRTEXTALIGNMENTEDITORWIDGET_H

#include "lrreportdesignwidget.h"
#include "lritemeditorwidget.h"
#include <QToolBar>
#include <QAction>

namespace LimeReport{

class TextAlignmentEditorWidget:public ItemEditorWidget
{
    Q_OBJECT
public:
    explicit TextAlignmentEditorWidget(const QString &title, QWidget *parent = 0);
    int flag() const;
protected:
    void setItemEvent(BaseDesignIntf *item);
    void initEditor();
    bool m_textAttibutesIsChanging;
private:
    void updateValues(const Qt::Alignment& align);
    Qt::Alignment createAlignment();
protected slots:
    virtual void slotTextHAttribsChanged(bool);
    virtual void slotTextVAttribsChanged(bool);
    virtual void slotPropertyChanged(const QString& objectName, const QString& property, const QVariant &oldValue, const QVariant &newValue);
private:
    QAction* m_textAliginLeft;
    QAction* m_textAliginRight;
    QAction* m_textAliginHCenter;
    QAction* m_textAliginJustify;
    QAction* m_textAliginTop;
    QAction* m_textAliginBottom;
    QAction* m_textAliginVCenter;
    int m_flag;
};

class TextAlignmentEditorWidgetForPage: public TextAlignmentEditorWidget{
    Q_OBJECT
public:
    TextAlignmentEditorWidgetForPage(PageDesignIntf* page, const QString &title, QWidget *parent = 0)
        :TextAlignmentEditorWidget(title, parent), m_page(page){}
protected:
    void initEditor();
protected slots:
    void slotTextHAttribsChanged(bool value);
    void slotTextVAttribsChanged(bool value);
private:
    PageDesignIntf* m_page;
};

#ifdef HAVE_REPORT_DESIGNER
class TextAlignmentEditorWidgetForDesigner: public TextAlignmentEditorWidget{
    Q_OBJECT
public:
    TextAlignmentEditorWidgetForDesigner(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0)
        :TextAlignmentEditorWidget(title, parent), m_reportEditor(reportEditor){initEditor();}
protected:
    void initEditor();
protected slots:
    void slotTextHAttribsChanged(bool value);
    void slotTextVAttribsChanged(bool value);
private:
    ReportDesignWidget* m_reportEditor;
};
#endif

} //namespace LimeReport

#endif // LRTEXTALIGNMENTEDITORWIDGET_H
