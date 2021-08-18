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
#ifndef LRCOMBOBOXEDITOR_H
#define LRCOMBOBOXEDITOR_H

#include <QWidget>
#include <QComboBox>
//#include <QPushButton>

class QToolButton;

namespace LimeReport{

class InternalComboBox :public QComboBox{
    Q_OBJECT
public:
    InternalComboBox(QWidget* parent=0):QComboBox(parent),m_popup(false){}
    void showPopup(){m_popup = true;QComboBox::showPopup();}
    void hidePopup(){QComboBox::hidePopup(); m_popup = false;}
    bool isPopup(){return m_popup;}
private:
    bool m_popup;

};

class ComboBoxEditor : public QWidget
{
    Q_OBJECT
public:
    //explicit ComboBoxEditor(QWidget *parent = 0);
    ComboBoxEditor(QWidget *parent=0, bool clearable=false);
    void addItems(const QStringList& values);
    void setTextValue(const QString& value);
    QString text();
    void setEditable(bool value);
signals:
    void editingFinished();
    void currentIndexChanged(const QString&);
protected:
    void resizeEvent(QResizeEvent *e);
private slots:
    void slotClearButtonClicked();
    void slotCurrentIndexChanged(const QString& value);
private:
    bool eventFilter(QObject *target, QEvent *event);
    InternalComboBox* m_comboBox;
    QToolButton* m_buttonClear;
    bool m_settingValues;
};

} // namespace LimeReport

#endif // LRCOMBOBOXEDITOR_H
