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
#ifndef LRPREVIEWREPORTWINDOW_H
#define LRPREVIEWREPORTWINDOW_H

#include <QMainWindow>
#include <QDomComment>
#include <QSpinBox>
#include <QComboBox>

#include "lrpagedesignintf.h"
#include "lrreportrender.h"
#include "serializators/lrstorageintf.h"
#include "serializators/lrxmlreader.h"
#include "lrpreviewreportwidget.h"

#include "items/editors/lrfonteditorwidget.h"
#include "items/editors/lrtextalignmenteditorwidget.h"

namespace LimeReport {

namespace Ui {
class PreviewReportWindow;
}

class PreviewReportWindow : public QMainWindow
{
    Q_OBJECT   
public:
    explicit PreviewReportWindow(ReportEnginePrivate *report, QWidget *parent = 0, QSettings* settings=0, Qt::WindowFlags flags=0);
    ~PreviewReportWindow();
    void setReportReader(ItemsReaderIntf::Ptr reader);
    void setPages(ReportPages pages);
    void exec();
    void initPreview(int pagesCount);
    void reloadPreview();
    void setSettings(QSettings* value);
    void setErrorMessages(const QStringList& value);
    void setToolBarVisible(bool value);
    void setStatusBarVisible(bool value);
    void setMenuVisible(bool value);
    void setHideResultEditButton(bool value);
    QSettings* settings();
    ScaleType previewScaleType() const;
    void setPreviewScaleType(const ScaleType &previewScaleType, int percent = 0);

protected:
    void writeSetting();
    void restoreSetting();
    void closeEvent(QCloseEvent *);
    void resizeEvent(QResizeEvent *e);
    void moveEvent(QMoveEvent *e);
    void showEvent(QShowEvent *);
    void selectStateIcon();
public slots:
    void slotPrint();
    void slotPriorPage();
    void slotNextPage();
    void slotZoomIn();
    void slotZoomOut();
    void slotPageNavigatorChanged(int value);
    void slotShowErrors();
    void on_actionSaveToFile_triggered();
    void slotSelectionChanged();
    void on_actionEdit_Mode_triggered(bool checked);
    void slotFirstPage();
    void slotLastPage();
    void slotPrintToPDF();
    void slotPageChanged(int pageIndex);
private slots:
    void on_actionFit_page_width_triggered();
    void on_actionFit_page_triggered();
    void on_actionOne_to_one_triggered();
    void scaleComboboxChanged(QString text);
    void slotScalePercentChanged(int percent);    
    void on_actionShowMessages_toggled(bool value);
    void on_actionShow_Toolbar_triggered();
private:
    ItemsReaderIntf* reader();
    void initPercentCombobox();
private:
    Ui::PreviewReportWindow *ui;
    QSpinBox* m_pagesNavigator;
    QSharedPointer<ItemsReaderIntf> m_reader;
    QEventLoop m_eventLoop;
    bool m_changingPage;
    QSettings* m_settings;
    bool m_ownedSettings;
    FontEditorWidget* m_fontEditor;
    TextAlignmentEditorWidget* m_textAlignmentEditor;
    int m_priorScrolValue;
    PreviewReportWidget* m_previewReportWidget;
    QComboBox* m_scalePercent;
    ScaleType m_previewScaleType;
    int m_previewScalePercent;
    bool m_scalePercentChanging;

};
} //namespace LimeReport
#endif // LRPREVIEWREPORTWINDOW_H
