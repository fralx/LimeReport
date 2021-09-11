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
#ifndef LRTEXTITEMEDITOR_H
#define LRTEXTITEMEDITOR_H

#include <QWidget>
#include <QTextEdit>
#include <QCompleter>

#include "lrtextitem.h"
#include "lrpagedesignintf.h"

namespace LimeReport{

namespace Ui {
class TextItemEditor;
}

//class CompleaterTextEditor :public QTextEdit
//{
//    Q_OBJECT
//public:
//    CompleaterTextEditor(QWidget* parent=0);
//    void setCompleter(QCompleter* value);
//    QCompleter* compleater() const{ return m_compleater;}
//protected:
//    virtual void keyPressEvent(QKeyEvent *e);
//    virtual void focusInEvent(QFocusEvent *e);
//private:
//    QString textUnderCursor() const;
//private slots:
//    void insertCompletion(const QString& completion);
//private:
//    QCompleter* m_compleater;
//};

class TextItemEditor : public QWidget
{
    Q_OBJECT  
public:
    explicit TextItemEditor(LimeReport::TextItem* item, LimeReport::PageDesignIntf* page,
                             QSettings* settings=0, QWidget *parent = 0);
    ~TextItemEditor();
    void setSettings(QSettings* value);
    QSettings* settings(); 
protected:
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void closeEvent(QCloseEvent *event);
    BandDesignIntf* findParentBand();
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void slotSplitterMoved(int, int);
private:
    void initUI();
    void readSetting();
    void writeSetting();
private:
    Ui::TextItemEditor *ui;
    LimeReport::TextItem* m_textItem;
    LimeReport::PageDesignIntf* m_page;
    QSettings* m_settings;
    bool m_ownedSettings;
    bool m_isReadingSetting;
};

} // namespace LimeReport

#endif // LRTEXTITEMEDITOR_H
