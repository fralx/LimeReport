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
#include "lritemsaligneditorwidget.h"

namespace LimeReport{

ItemsAlignmentEditorWidget::ItemsAlignmentEditorWidget(LimeReport::ReportDesignWidget* reportEditor, const QString& title, QWidget* parent)
    :QToolBar(title,parent), m_reportEditor(reportEditor), m_page(0)
{
    initEditor();
}

ItemsAlignmentEditorWidget::ItemsAlignmentEditorWidget(ReportDesignWidget* reportEditor, QWidget* parent)
    :QToolBar(parent), m_reportEditor(reportEditor), m_page(0)
{
    initEditor();
}

ItemsAlignmentEditorWidget::ItemsAlignmentEditorWidget(PageDesignIntf* page, const QString& title, QWidget* parent)
    :QToolBar(title,parent), m_reportEditor(0), m_page(page)
{
    initEditor();
}

ItemsAlignmentEditorWidget::ItemsAlignmentEditorWidget(PageDesignIntf* page, QWidget* parent)
    :QToolBar(parent), m_reportEditor(0), m_page(page)
{
    initEditor();
}

void ItemsAlignmentEditorWidget::slotBringToFront()
{
    if (m_reportEditor) m_reportEditor->bringToFront();
    if (m_page) m_page->bringToFront();
}

void ItemsAlignmentEditorWidget::slotSendToBack()
{
    if (m_reportEditor) m_reportEditor->sendToBack();
    if (m_page) m_page->sendToBack();
}

void ItemsAlignmentEditorWidget::slotAlignToLeft()
{
    if (m_reportEditor) m_reportEditor->alignToLeft();
    if (m_page) m_page->alignToLeft();
}

void ItemsAlignmentEditorWidget::slotAlignToRight()
{
    if (m_reportEditor) m_reportEditor->alignToRight();
    if (m_page) m_page->alignToRigth();
}

void ItemsAlignmentEditorWidget::slotAlignToVCenter()
{
    if (m_reportEditor) m_reportEditor->alignToVCenter();
    if (m_page) m_page->alignToVCenter();
}

void ItemsAlignmentEditorWidget::slotAlignToTop()
{
    if (m_reportEditor) m_reportEditor->alignToTop();
    if (m_page) m_page->alignToTop();
}

void ItemsAlignmentEditorWidget::slotAlignToBottom()
{
    if (m_reportEditor) m_reportEditor->alignToBottom();
    if (m_page) m_page->alignToBottom();
}

void ItemsAlignmentEditorWidget::slotAlignToHCenter()
{
    if (m_reportEditor) m_reportEditor->alignToHCenter();
    if (m_page) m_page->alignToHCenter();
}

void ItemsAlignmentEditorWidget::slotSameHeight()
{
    if (m_reportEditor) m_reportEditor->sameHeight();
    if (m_page) m_page->sameHeight();
}

void ItemsAlignmentEditorWidget::slotSameWidth()
{
    if (m_reportEditor) m_reportEditor->sameWidth();
    if (m_page) m_page->sameWidth();
}

void ItemsAlignmentEditorWidget::initEditor()
{
    m_bringToFront = new QAction(tr("Bring to top"),this);
    m_bringToFront->setIcon(QIcon(":/report/images/bringToTop"));
    connect(m_bringToFront,SIGNAL(triggered()),this,SLOT(slotBringToFront()));
    addAction(m_bringToFront);

    m_sendToBack = new QAction(tr("Send to back"),this);
    m_sendToBack->setIcon(QIcon(":/report/images/sendToBack"));
    connect(m_sendToBack,SIGNAL(triggered()),this,SLOT(slotSendToBack()));
    addAction(m_sendToBack);

    m_alignToLeft = new QAction(tr("Align to left"),this);
    m_alignToLeft->setIcon(QIcon(":/report/images/alignToLeft"));
    connect(m_alignToLeft,SIGNAL(triggered()),this,SLOT(slotAlignToLeft()));
    addAction(m_alignToLeft);

    m_alignToRight = new QAction(tr("Align to right"),this);
    m_alignToRight->setIcon(QIcon(":/report/images/alignToRight"));
    connect(m_alignToRight,SIGNAL(triggered()),this,SLOT(slotAlignToRight()));
    addAction(m_alignToRight);

    m_alignToVCenter = new QAction(tr("Align to vertical center"),this);
    m_alignToVCenter->setIcon(QIcon(":/report/images/alignToVCenter"));
    connect(m_alignToVCenter,SIGNAL(triggered()),this,SLOT(slotAlignToVCenter()));
    addAction(m_alignToVCenter);

    m_alignToTop = new QAction(tr("Align to top"),this);
    m_alignToTop->setIcon(QIcon(":/report/images/alignToTop"));
    connect(m_alignToTop,SIGNAL(triggered()),this,SLOT(slotAlignToTop()));
    addAction(m_alignToTop);

    m_alignToBottom = new QAction(tr("Align to bottom"),this);
    m_alignToBottom->setIcon(QIcon(":/report/images/alignToBottom"));
    connect(m_alignToBottom,SIGNAL(triggered()),this,SLOT(slotAlignToBottom()));
    addAction(m_alignToBottom);

    m_alignToHCenter = new QAction(tr("Align to horizontal center"),this);
    m_alignToHCenter->setIcon(QIcon(":/report/images/alignToHCenter"));
    connect(m_alignToHCenter,SIGNAL(triggered()),this,SLOT(slotAlignToHCenter()));
    addAction(m_alignToHCenter);

    m_sameHeight = new QAction(tr("Set same height"),this);
    m_sameHeight->setIcon(QIcon(":/report/images/sameHeight"));
    connect(m_sameHeight,SIGNAL(triggered()),this,SLOT(slotSameHeight()));
    addAction(m_sameHeight);

    m_sameWidth = new QAction(tr("Set same width"),this);
    m_sameWidth->setIcon(QIcon(":/report/images/sameWidth"));
    connect(m_sameWidth,SIGNAL(triggered()),this,SLOT(slotSameWidth()));
    addAction(m_sameWidth);
}

} //namespace LimeReport
