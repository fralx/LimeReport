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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQueryModel>
#include "lrreportengine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void renderStarted();
    void renderPageFinished(int renderedPageCount);
    void renderFinished();
    void slotGetCallbackData(LimeReport::CallbackInfo info, QVariant& data);
    void slotChangePos(const LimeReport::CallbackInfo::ChangePosType& type, bool& result);
    void slotGetCallbackChildData(LimeReport::CallbackInfo info, QVariant& data);
    void slotChangeChildPos(const LimeReport::CallbackInfo::ChangePosType& type, bool& result);
    void slotOneSlotDS(LimeReport::CallbackInfo info, QVariant& data);
private:
    void prepareData(QSqlQuery* ds, LimeReport::CallbackInfo info, QVariant &data);
private:
    Ui::MainWindow *ui;
    LimeReport::ReportEngine *report;
    QProgressDialog* m_progressDialog;
    int m_currentPage;
    QSqlDatabase m_db;
    int m_currentMasterRecord;
    QVector<QString> m_childData;

    int m_currentCustomerRecord;
    int m_currentOrderRecord;
    QSqlQuery* m_customers;
    QSqlQuery* m_orders;

};



#endif // MAINWINDOW_H
