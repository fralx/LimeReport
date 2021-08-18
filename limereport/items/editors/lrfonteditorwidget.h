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
#ifndef LRFONTEDITORWIDGET_H
#define LRFONTEDITORWIDGET_H

#include <QToolBar>
#include <QFontComboBox>
#include <QStringListModel>
#include <QAction>

#ifdef HAVE_REPORT_DESIGNER
#include "lrreportdesignwidget.h"
#endif

#include "lritemeditorwidget.h"

namespace LimeReport{

class FontEditorWidget :public ItemEditorWidget{
    Q_OBJECT
public:
    explicit FontEditorWidget(const QString &title, QWidget *parent = 0);
    bool ignoreSlots() const;
protected:
    void setItemEvent(BaseDesignIntf *item);
    QFontComboBox* fontNameEditor(){return m_fontNameEditor;}
    void initEditor();
protected slots:
    virtual void slotFontChanged(const QFont&);
    virtual void slotFontSizeChanged(const QString& value);
    virtual void slotFontAttribsChanged(bool);
    void slotPropertyChanged(const QString& objectName, const QString& property, const QVariant &oldValue, const QVariant &newValue);
protected:
    QFont resFont(){return m_resFont;}
private:

    void updateValues(const QFont &font);

    QFontComboBox* m_fontNameEditor;
    QComboBox* m_fontSizeEditor;
    QStringListModel m_fontSizeModel;

    QAction* m_fontBold;
    QAction* m_fontItalic;
    QAction* m_fontUnderline;

    bool m_ignoreSlots;
    QFont m_resFont;

};

class FontEditorWidgetForPage : public FontEditorWidget{
    Q_OBJECT
public:
    explicit FontEditorWidgetForPage(PageDesignIntf* page, const QString &title, QWidget *parent = 0)
        : FontEditorWidget(title, parent), m_page(page){}
protected slots:
    virtual void slotFontChanged(const QFont& font);
    virtual void slotFontSizeChanged(const QString& value);
    virtual void slotFontAttribsChanged(bool value);
private:
    PageDesignIntf* m_page;
};

#ifdef HAVE_REPORT_DESIGNER
class FontEditorWidgetForDesigner : public FontEditorWidget{
    Q_OBJECT
public:
    explicit FontEditorWidgetForDesigner(ReportDesignWidget* reportEditor, const QString &title, QWidget *parent = 0)
        : FontEditorWidget(title, parent), m_reportEditor(reportEditor){initEditor();}

protected:
    void initEditor();
protected slots:
    virtual void slotFontChanged(const QFont& font);
    virtual void slotFontSizeChanged(const QString& value);
    virtual void slotFontAttribsChanged(bool value);
private:
    ReportDesignWidget* m_reportEditor;
};
#endif

} //namespace LimeReport

#endif // LRFONTEDITORWIDGET_H
