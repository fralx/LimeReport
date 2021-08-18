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
#ifndef LRSCRIPTBROWSER_H
#define LRSCRIPTBROWSER_H

#include <QWidget>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include "lrreportdesignwidget.h"

namespace LimeReport{

namespace Ui {
class ScriptBrowser;
}

class ScriptBrowser : public QWidget
{
    Q_OBJECT
    
public:
    explicit ScriptBrowser(QWidget *parent = 0);
    ~ScriptBrowser();
    void setReportEditor(LimeReport::ReportDesignWidget* designerWidget);
    inline ReportDesignWidget* reportEditor(){return m_designerWidget;}
    void updateFunctionTree();
#ifdef HAVE_UI_LOADER
    void updateDialogsTree();
#endif
protected:
#ifdef HAVE_UI_LOADER
    void fillDialog(QTreeWidgetItem *dialogItem, const QString &description);
    void fillProperties(QTreeWidgetItem *objectItem, QObject *item);
#endif
private slots:
    void slotClear();
    void slotUpdate();
#ifdef HAVE_UI_LOADER
    void slotDialogAdded(QString);
    void on_tbAddDialog_clicked();
    void on_tbRunDialog_clicked();
    void on_tbDeleteDialog_clicked();
#endif

private:
    Ui::ScriptBrowser *ui;
    ReportDesignWidget*  m_designerWidget;
};

} // namespace LimeReport
#endif // LRSCRIPTBROWSER_H
