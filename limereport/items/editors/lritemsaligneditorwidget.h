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
#ifndef LRITEMSALIGNEDITORWIDGET_H
#define LRITEMSALIGNEDITORWIDGET_H

#include "lrreportdesignwidget.h"
#include <QToolBar>
#include <QAction>

namespace LimeReport{

class ItemsAlignmentEditorWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit ItemsAlignmentEditorWidget(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0);
    explicit ItemsAlignmentEditorWidget(ReportDesignWidget* reportEditor, QWidget *parent = 0);
    explicit ItemsAlignmentEditorWidget(PageDesignIntf* page, const QString &title, QWidget *parent = 0);
    explicit ItemsAlignmentEditorWidget(PageDesignIntf* page, QWidget *parent = 0);
private slots:
    void slotBringToFront();
    void slotSendToBack();
    void slotAlignToLeft();
    void slotAlignToRight();
    void slotAlignToVCenter();
    void slotAlignToTop();
    void slotAlignToBottom();
    void slotAlignToHCenter();
    void slotSameHeight();
    void slotSameWidth();
private:
    void initEditor();
    ReportDesignWidget* m_reportEditor;
    PageDesignIntf* m_page;

    QAction* m_bringToFront;
    QAction* m_sendToBack;
    QAction* m_alignToLeft;
    QAction* m_alignToRight;
    QAction* m_alignToVCenter;
    QAction* m_alignToTop;
    QAction* m_alignToBottom;
    QAction* m_alignToHCenter;
    QAction* m_sameHeight;
    QAction* m_sameWidth;
};

} //namespace LimeReport

#endif // LRITEMSALIGNEDITORWIDGET_H
