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
#ifndef LRFONTEDITORWIDGET_H
#define LRFONTEDITORWIDGET_H

#include <QToolBar>
#include <QFontComboBox>
#include <QStringListModel>
#include <QAction>

#include "lrreportdesignwidget.h"
#include "lritemeditorwidget.h"

namespace LimeReport{

class FontEditorWidget :public ItemEditorWidget{
    Q_OBJECT
public:
    explicit FontEditorWidget(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0);
    explicit FontEditorWidget(ReportDesignWidget* reportEditor, QWidget *parent = 0);
    explicit FontEditorWidget(PageDesignIntf* page, const QString &title, QWidget *parent = 0);
    explicit FontEditorWidget(PageDesignIntf* page, QWidget *parent = 0);
protected:
    void setItemEvent(BaseDesignIntf *item);
    QFontComboBox* fontNameEditor(){return m_fontNameEditor;}
private slots:
    void slotFontChanged(const QFont& font);
    void slotFontSizeChanged(const QString& value);
    void slotFontAttribsChanged(bool);
    void slotPropertyChanged(const QString& objectName, const QString& property, const QVariant &oldValue, const QVariant &newValue);
private:
    void initEditor();
    void updateValues(const QFont &font);

    QFontComboBox* m_fontNameEditor;
    QComboBox* m_fontSizeEditor;
    QStringListModel m_fontSizeModel;

    QAction* m_fontBold;
    QAction* m_fontItalic;
    QAction* m_fontUnderline;

    bool m_ignoreSlots;

};

} //namespace LimeReport

#endif // LRFONTEDITORWIDGET_H
