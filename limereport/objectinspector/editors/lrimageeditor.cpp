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
#include <QHBoxLayout>
#include <QFileDialog>
#include "lrimageeditor.h"

namespace LimeReport{

ImageEditor::ImageEditor(QWidget* parent)
    :QWidget(parent)
{
    m_button.setIcon(QIcon(":items/ImageItem"));
    m_clearButton.setIcon(QIcon(":items/clear.png"));
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&m_button);
    layout->addWidget(&m_clearButton);
    layout->setSpacing(1);
    layout->setContentsMargins(1,0,1,1);
    setLayout(layout);
    setFocusProxy(&m_button);
    setAutoFillBackground(true);
    connect(&m_button,SIGNAL(clicked()),this,SLOT(slotButtonClicked()));
    connect(&m_clearButton,SIGNAL(clicked()),this,SLOT(slotClearButtonClicked()));
}

QImage ImageEditor::image()
{
    return m_image;
}

void ImageEditor::slotButtonClicked()
{
    m_image.load(QFileDialog::getOpenFileName(this));
    emit editingFinished();
}

void ImageEditor::slotClearButtonClicked()
{
    m_image = QImage();
    emit editingFinished();
}

} //namespace LimeReport
