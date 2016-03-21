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
    explicit TextAlignmentEditorWidget(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0);
    explicit TextAlignmentEditorWidget(ReportDesignWidget* reportEditor, QWidget *parent = 0);
    explicit TextAlignmentEditorWidget(PageDesignIntf* page, const QString &title, QWidget *parent = 0);
    explicit TextAlignmentEditorWidget(PageDesignIntf* page, QWidget *parent = 0);
protected:
    void setItemEvent(BaseDesignIntf *item);
private:
    void initEditor();
    void updateValues(const Qt::Alignment& align);
    Qt::Alignment createAlignment();
private slots:
    void slotTextHAttribsChanged(bool);
    void slotTextVAttribsChanged(bool);
    void slotPropertyChanged(const QString& objectName, const QString& property, const QVariant &oldValue, const QVariant &newValue);
private:
    bool m_textAttibutesIsChanging;

    QAction* m_textAliginLeft;
    QAction* m_textAliginRight;
    QAction* m_textAliginHCenter;
    QAction* m_textAliginJustify;
    QAction* m_textAliginTop;
    QAction* m_textAliginBottom;
    QAction* m_textAliginVCenter;

};

} //namespace LimeReport

#endif // LRTEXTALIGNMENTEDITORWIDGET_H
