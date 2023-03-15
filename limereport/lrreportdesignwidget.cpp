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
#include "lrreportdesignwidget.h"
#include "serializators/lrxmlreader.h"
#include "serializators/lrxmlwriter.h"
#include "lrreportengine_p.h"
#include "lrbasedesignintf.h"
#include "lrsettingdialog.h"
#include "dialogdesigner/lrdialogdesigner.h"
#include "translationeditor/translationeditor.h"
#include "scripteditor/lrscripteditor.h"

#include <QDebug>
#include <QObject>
#include <QDockWidget>
#include <QtXml>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QTabWidget>
#include <QMessageBox>

namespace LimeReport {

// ReportDesignIntf

ReportDesignWidget::ReportDesignWidget(ReportEnginePrivateInterface* report, QSettings* settings, QMainWindow *mainWindow, QWidget *parent) :
    QWidget(parent),
#ifdef HAVE_QTDESIGNER_INTEGRATION
    m_dialogDesignerManager(new DialogDesignerManager(this)),
#endif
    m_mainWindow(mainWindow), m_verticalGridStep(10), m_horizontalGridStep(10), m_useGrid(false),
    m_dialogChanged(false), m_theme("Default"), m_settings(settings), m_defaultUnits(BaseDesignIntf::Millimeters)
{
#if QT_VERSION < 0x050000
    m_tabWidget = new LimeReportTabWidget(this);
#else
    m_tabWidget = new QTabWidget(this);
#endif
    m_tabWidget->setTabPosition(QTabWidget::South);
    m_tabWidget->setMovable(true);
    connect(m_tabWidget->tabBar(), SIGNAL(tabMoved(int,int)), this, SLOT(slotTabMoved(int,int)));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    setLayout(mainLayout);

    m_report=report;

    m_settings->beginGroup("DesignerWidget");
    QVariant v = m_settings->value("DefaultUnits");
    if (v.isValid()){
        m_defaultUnits = static_cast<BaseDesignIntf::UnitType>(v.toInt());
    }
    m_settings->endGroup();

    if (!m_report->pageCount()){
        createStartPage();
    }

    createTabs();
    connect(dynamic_cast<QObject*>(m_report), SIGNAL(pagesLoadFinished()),this,SLOT(slotPagesLoadFinished()));
    connect(dynamic_cast<QObject*>(m_report), SIGNAL(cleared()), this, SIGNAL(cleared()));
    connect(dynamic_cast<QObject*>(m_report), SIGNAL(loadFinished()), this, SLOT(slotReportLoaded()));

    connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
#ifdef HAVE_UI_LOADER
    connect(m_report->scriptContext(), SIGNAL(dialogDeleted(QString)), this, SLOT(slotDialogDeleted(QString)));
#endif
    //m_instance=this;
    m_scriptEditor->setPlainText(m_report->scriptContext()->initScript());
    m_zoomer = new GraphicsViewZoomer(activeView());

#ifdef Q_OS_WIN
    m_defaultFont = QFont("Arial",10);
#endif

#ifdef HAVE_QTDESIGNER_INTEGRATION
    connect(m_dialogDesignerManager, SIGNAL(dialogChanged(QString)),
            this, SLOT(slotDialogChanged(QString)));
    connect(m_dialogDesignerManager, SIGNAL(dialogNameChanged(QString,QString)),
            this, SLOT(slotDialogNameChanged(QString,QString)));
#endif

    m_themes.insert("Default","");
    m_localToEng.insert(QObject::tr("Dark"), "Dark");
    m_localToEng.insert(QObject::tr("Light"), "Light");
    initThemeIfExist("Dark", ":/qdarkstyle/style.qss");
    initThemeIfExist("Light", ":/qlightstyle/lightstyle.qss");
}

#ifdef HAVE_QTDESIGNER_INTEGRATION
DialogDesignerManager *ReportDesignWidget::dialogDesignerManager() const
{
    return m_dialogDesignerManager;
}

QString ReportDesignWidget::activeDialogName()
{
    if (activeDialogPage())
     return activeDialogPage()->dialogName();
    return "";
}


QWidget *ReportDesignWidget::toolWindow(ReportDesignWidget::ToolWindowType windowType)
{
    switch (windowType) {
    case WidgetBox:
        return dialogDesignerManager()->widgetBox();
    case PropertyEditor:
        return dialogDesignerManager()->propertyEditor();
    case ObjectInspector:
        return dialogDesignerManager()->objectInspector();
    case ActionEditor:
        return dialogDesignerManager()->actionEditor();
    case ResourceEditor:
        return dialogDesignerManager()->resourcesEditor();
    case SignalSlotEditor:
        return dialogDesignerManager()->signalSlotEditor();
    }
    return NULL;
}

#endif

ReportDesignWidget::EditorTabType ReportDesignWidget::activeTabType()
{
    QString tabType = m_tabWidget->tabWhatsThis(m_tabWidget->currentIndex());
    if ( tabType.compare("dialog") == 0) return Dialog;
    if ( tabType.compare("script") == 0) return Script;
    if ( tabType.compare("translations") == 0) return Translations;
    return Page;
}

#ifdef HAVE_QTDESIGNER_INTEGRATION

void ReportDesignWidget::initDialogDesignerToolBar(QToolBar *toolBar)
{
    m_dialogDesignerManager->initToolBar(toolBar);
}

void ReportDesignWidget::updateDialogs()
{
    for ( int i = 0; i<m_tabWidget->count(); ++i ){
        if (m_tabWidget->tabWhatsThis(i).compare("dialog") == 0){
            m_report->scriptContext()->changeDialog(m_tabWidget->tabText(i), m_dialogDesignerManager->getDialogDescription(m_tabWidget->widget(i)));
        }
    }
}

#endif

bool ReportDesignWidget::useMagnet() const
{
    return m_useMagnet;
}

void ReportDesignWidget::setUseMagnet(bool useMagnet)
{
    m_useMagnet = useMagnet;
    for (int i=0;i<m_report->pageCount();++i){
        m_report->pageAt(i)->setMagneticMovement(useMagnet);
    }
}

void ReportDesignWidget::saveState()
{
    m_settings->beginGroup("DesignerWidget");
    m_settings->setValue("hGridStep",m_horizontalGridStep);
    m_settings->setValue("vGridStep",m_verticalGridStep);
    m_settings->setValue("defaultFont",m_defaultFont);
    m_settings->setValue("useGrid",m_useGrid);
    m_settings->setValue("theme",m_theme);
    m_settings->setValue("ScriptEditorState", m_scriptEditor->saveState());
    m_settings->setValue("DefaultUnits", m_defaultUnits);
    m_settings->endGroup();
}

void ReportDesignWidget::applySettings()
{
    for (int i=0;i<m_report->pageCount();++i){
        m_report->pageAt(i)->pageItem()->setFont(m_defaultFont);
    }
    applyUseGrid();

    if (m_themes.contains(m_theme)){
        parentWidget()->setStyleSheet(m_themes.value(m_theme));
        m_report->setStyleSheet(m_themes.value(m_theme));
    } else {
        m_theme = "Default";
        parentWidget()->setStyleSheet("");
        m_report->setStyleSheet("");
    }

//    if (m_theme.compare("Dark") == 0) {
//        QFile theme(":/qdarkstyle/style.qss");
//        theme.open(QIODevice::ReadOnly);
//        QString styleSheet = theme.readAll();
//        parentWidget()->setStyleSheet(styleSheet);
//        m_report->setStyleSheet(styleSheet);
//    } else {
//        parentWidget()->setStyleSheet("");
//        m_report->setStyleSheet("");
//    }

    if (m_settings){
        m_settings->beginGroup("ScriptEditor");
        QVariant v = m_settings->value("DefaultFontName");
        if (v.isValid()){
            QVariant fontSize = m_settings->value("DefaultFontSize");
            m_scriptEditor->setEditorFont(QFont(v.toString(),fontSize.toInt()));
        }
        v =  m_settings->value("TabIndention");
        if (v.isValid()){
            m_scriptEditor->setTabIndention(v.toInt());
        }
        m_settings->endGroup();
    }
}

void ReportDesignWidget::loadState()
{
    m_settings->beginGroup("DesignerWidget");
    QVariant v = m_settings->value("hGridStep");
    if (v.isValid()){
        m_horizontalGridStep = v.toInt();
    }

    v = m_settings->value("vGridStep");
    if (v.isValid()){
        m_verticalGridStep = v.toInt();
    }
    v = m_settings->value("defaultFont");
    if (v.isValid()){
        m_defaultFont = v.value<QFont>();
    }

    v = m_settings->value("useGrid");
    if (v.isValid()){
        m_useGrid = v.toBool();
    }

    v = m_settings->value("theme");
    if (v.isValid()){
        m_theme = v.toString();
    }

    v = m_settings->value("ScriptEditorState");
    if (v.isValid() && m_scriptEditor){
        m_scriptEditor->restoreState(v.toByteArray());
    }

    v = m_settings->value("DefaultUnits");
    if (v.isValid()){
        m_defaultUnits = static_cast<BaseDesignIntf::UnitType>(v.toInt());
    }

    m_settings->endGroup();
    applySettings();
}

PageView* ReportDesignWidget::createPageView(PageDesignIntf* page){
    PageView* view = new PageView(this);
    view->setBackgroundBrush(QBrush(Qt::gray));
    view->setFrameShape(QFrame::NoFrame);
    view->setScene(page);
    view->setPageItem(page->pageItem());
    view->scale(0.5, 0.5);
    view->centerOn(0, 0);
    return view;
}

void ReportDesignWidget::createTabs(){
    m_tabWidget->clear();
    int pageIndex  = -1;

    for (int i = 0; i < m_report->pageCount(); ++i){
        PageDesignIntf* page = m_report->pageAt(i);
        page->clearSelection();
        connectPage(page);
        PageView* view = createPageView(page);
        int pageIndex = m_tabWidget->addTab(view, QIcon(), page->pageItem()->objectName());
        m_tabWidget->setTabWhatsThis(pageIndex, "page");
    }

    m_scriptEditor = new ScriptEditor(this);
    connect(m_scriptEditor, SIGNAL(textChanged()), this, SLOT(slotScriptTextChanged()));
    m_scriptEditor->setReportEngine(m_report);
    pageIndex = m_tabWidget->addTab(m_scriptEditor,QIcon(),tr("Script"));
    m_tabWidget->setTabWhatsThis(pageIndex,"script");
    m_tabWidget->setCurrentIndex(0);

#ifdef HAVE_QTDESIGNER_INTEGRATION
    QWidget* dialogDesigner;
    foreach(DialogDescriber::Ptr dialogDesc, m_report->scriptContext()->dialogDescribers()){
        dialogDesigner = m_dialogDesignerManager->createFormEditor(dialogDesc->description());
        pageIndex = m_tabWidget->addTab(dialogDesigner,QIcon(),dialogDesc->name());
        m_tabWidget->setTabWhatsThis(pageIndex,"dialog");
    }
#endif

    m_traslationEditor = new TranslationEditor(this);
    pageIndex = m_tabWidget->addTab(m_traslationEditor,QIcon(),tr("Translations"));
    m_tabWidget->setTabWhatsThis(pageIndex,"translations");

}

#ifdef HAVE_QTDESIGNER_INTEGRATION
void ReportDesignWidget::createNewDialogTab(const QString& dialogName, const QByteArray& description)
{
    QWidget* dialogDesigner = m_dialogDesignerManager->createFormEditor(description);
    int pageIndex = m_tabWidget->addTab(dialogDesigner,QIcon(),dialogName);
    m_tabWidget->setTabWhatsThis(pageIndex,"dialog");
    m_tabWidget->setCurrentIndex(pageIndex);
    m_dialogDesignerManager->setActiveEditor(dialogDesigner);
}

DialogDesigner*ReportDesignWidget::activeDialogPage()
{
    return dynamic_cast<DialogDesigner*>(m_tabWidget->currentWidget());
}
#endif

ReportDesignWidget::~ReportDesignWidget()
{
    delete m_zoomer;
}

QGraphicsView* ReportDesignWidget::activeView(){
    return dynamic_cast<QGraphicsView*>(m_tabWidget->currentWidget());
}

void ReportDesignWidget::connectPage(PageDesignIntf *page)
{
    connect(page, SIGNAL(itemInserted(LimeReport::PageDesignIntf*, QPointF, QString)),
            this, SIGNAL(itemInserted(LimeReport::PageDesignIntf*, QPointF, QString)));
    connect(page, SIGNAL(itemInsertCanceled(QString)),this,SIGNAL(itemInsertCanceled(QString)));
    connect(page, SIGNAL(itemPropertyChanged(QString, QString, QVariant, QVariant)),
            this, SIGNAL(itemPropertyChanged(QString, QString, QVariant, QVariant)));
    connect(page, SIGNAL(itemPropertyObjectNameChanged(QString, QString)),
            this, SLOT(slotItemPropertyObjectNameChanged(QString, QString)));
    connect(page, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
    connect(page, SIGNAL(insertModeStarted()), this, SIGNAL(insertModeStarted()));
    connect(page, SIGNAL(commandHistoryChanged()), this, SIGNAL(commandHistoryChanged()));
    connect(page, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(slotSceneRectChanged(QRectF)));
    connect(page, SIGNAL(itemAdded(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*)),
            this, SIGNAL(itemAdded(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*)));
    connect(page, SIGNAL(itemRemoved(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*)),
            this, SIGNAL(itemDeleted(LimeReport::PageDesignIntf*, LimeReport::BaseDesignIntf*)));
    connect(page, SIGNAL(bandAdded(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*)),
            this, SIGNAL(bandAdded(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*)));
    connect(page, SIGNAL(bandRemoved(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*)),
            this, SIGNAL(bandDeleted(LimeReport::PageDesignIntf*, LimeReport::BandDesignIntf*)));
    connect(page, SIGNAL(pageUpdateFinished(LimeReport::PageDesignIntf*)),
            this, SIGNAL(activePageUpdated(LimeReport::PageDesignIntf*)));
    connect(page->pageItem(), SIGNAL(propertyObjectNameChanged(QString, QString)),
            this, SLOT(slotPagePropertyObjectNameChanged(QString, QString)));
    emit activePageChanged();
}

PageDesignIntf* ReportDesignWidget::createStartPage()
{
    PageDesignIntf* page = m_report->appendPage("page1");
    page->pageItem()->setUnitType(m_defaultUnits);
//    createTabs();
    return page;
}

void ReportDesignWidget::removeDatasource(const QString &datasourceName)
{
    if (m_report->dataManager())
        m_report->dataManager()->removeDatasource(datasourceName);
}

void ReportDesignWidget::addBand(const QString &bandType)
{
    if (activePage())
        activePage()->addBand(bandType);
}

void ReportDesignWidget::addBand(BandDesignIntf::BandsType bandType)
{
    if (activePage())
        activePage()->addBand(bandType);
}

void ReportDesignWidget::startInsertMode(const QString &itemType)
{
    if (activePage())
        activePage()->startInsertMode(itemType);
}

void ReportDesignWidget::startEditMode()
{
    if (activePage())
        activePage()->startEditMode();
}

PageDesignIntf * ReportDesignWidget::activePage()
{
    if (activeView())
        return dynamic_cast<PageDesignIntf*>(activeView()->scene());
    return 0;
}

QList<QGraphicsItem *> ReportDesignWidget::selectedItems(){
    return activePage()->selectedItems();
}

void ReportDesignWidget::deleteItem(QGraphicsItem *item){
    activePage()->removeReportItem(dynamic_cast<BaseDesignIntf*>(item));
}

void ReportDesignWidget::deleteSelectedItems(){
    activePage()->deleteSelected();
}

QStringList ReportDesignWidget::datasourcesNames(){
    return m_report->dataManager()->dataSourceNames();
}

void ReportDesignWidget::slotItemSelected(BaseDesignIntf *item){
    emit itemSelected(item);
}

bool ReportDesignWidget::saveToFile(const QString &fileName){

    bool result = false;
    prepareReport();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    updateDialogs();
#endif

    if (m_report->saveToFile(fileName)) {
        m_report->emitSaveFinished();
        result = true;
    }

#ifdef HAVE_QTDESIGNER_INTEGRATION
    if (result){
        m_dialogChanged = false;
        m_dialogDesignerManager->setDirty(false);
    }
#endif
    return result;
}

bool ReportDesignWidget::save()
{
    prepareReport();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    updateDialogs();
#endif

    bool result = false;

    if (emitSaveReport()) {
        result = true; // saved via signal
    }
    else if (!m_report->reportFileName().isEmpty()){
        if (m_report->saveToFile()){
            m_report->emitSaveFinished();
            result = true;
        }
    } else {
        if (m_report->isSaved()) {
            m_report->emitSaveFinished();
            result = true;
        }
        else if (m_report->saveToFile(QFileDialog::getSaveFileName(this,tr("Report file name"),"","Report files (*.lrxml);; All files (*)"))){
            m_report->emitSaveFinished();
            result = true;
        };
    }

#ifdef HAVE_QTDESIGNER_INTEGRATION
    if (result){
        m_dialogChanged = false;
        m_dialogDesignerManager->setDirty(false);
    }
#endif
    return result;
}

bool ReportDesignWidget::loadFromFile(const QString &fileName)
{
    if (m_report->loadFromFile(fileName,false)){
//        QByteArray editorState = m_scriptEditor->saveState();
//        createTabs();
//        m_scriptEditor->setPlainText(m_report->scriptContext()->initScript());
//        m_scriptEditor->restoreState(editorState);
//        emit loaded();
//        m_dialogChanged = false;
        return true;
    } else {
        QMessageBox::critical(this,tr("Error"),tr("Wrong file format"));
        return false;
    }
}

void ReportDesignWidget::scale(qreal sx, qreal sy)
{
    //m_view->scale(sx,sy);
    if (activeView()) activeView()->scale(sx,sy);
}

QString ReportDesignWidget::reportFileName()
{
    if (m_report)
        return m_report->reportFileName();
    return QString();
}

bool ReportDesignWidget::isNeedToSave()
{
    if(m_report)
        return (m_report->isNeedToSave() || m_dialogChanged);
    return false;
}

bool ReportDesignWidget::emitSaveReport()
{
    return m_report->emitSaveReport();
}

bool ReportDesignWidget::emitSaveReportAs()
{
    return m_report->emitSaveReportAs();
}

bool ReportDesignWidget::emitLoadReport()
{
    return m_report->emitLoadReport();
}

void ReportDesignWidget::updateSize()
{
    if (activePage())
        activePage()->slotUpdateItemSize();
}

void ReportDesignWidget::undo()
{
    if (activePage())
        activePage()->undo();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    if (activeDialogPage())
        activeDialogPage()->undo();
#endif
}

void ReportDesignWidget::redo()
{
    if (activePage())
        activePage()->redo();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    if (activeDialogPage())
        activeDialogPage()->redo();
#endif
}

void ReportDesignWidget::copy()
{
    if (activePage())
        activePage()->copy();
}

void ReportDesignWidget::paste()
{
    if (activePage())
        activePage()->paste();
}

void ReportDesignWidget::cut()
{
    if (activePage())
        activePage()->cut();
}

void ReportDesignWidget::bringToFront()
{
    if (activePage())
        activePage()->bringToFront();
}

void ReportDesignWidget::sendToBack()
{
    if (activePage())
        activePage()->sendToBack();
}

void ReportDesignWidget::alignToLeft()
{
    if (activePage())
        activePage()->alignToLeft();
}

void ReportDesignWidget::alignToRight()
{
    if (activePage())
        activePage()->alignToRigth();
}

void ReportDesignWidget::alignToVCenter()
{
    if (activePage())
        activePage()->alignToVCenter();
}

void ReportDesignWidget::alignToTop()
{
    if (activePage())
        activePage()->alignToTop();
}

void ReportDesignWidget::alignToBottom()
{
    if (activePage())
        activePage()->alignToBottom();
}

void ReportDesignWidget::alignToHCenter()
{
    if (activePage())
        activePage()->alignToHCenter();
}

void ReportDesignWidget::sameHeight()
{
    if (activePage())
        activePage()->sameHeight();
}

void ReportDesignWidget::sameWidth()
{
    if (activePage())
        activePage()->sameWidth();
}

void ReportDesignWidget::editLayoutMode(bool value)
{
    if (value ){
        activePage()->pageItem()->setItemMode(activePage()->pageItem()->itemMode() | LayoutEditMode);
    } else if (activePage()->pageItem()->itemMode() & LayoutEditMode){
        activePage()->pageItem()->setItemMode(activePage()->pageItem()->itemMode() ^ LayoutEditMode);
    }
}

void ReportDesignWidget::addHLayout()
{
    if (activePage())
        activePage()->addHLayout();
}

void ReportDesignWidget::addVLayout()
{
    if (activePage())
        activePage()->addVLayout();
}

void ReportDesignWidget::setFont(const QFont& font)
{
    if (activePage())
        activePage()->setFont(font);
}

void ReportDesignWidget::setTextAlign(const bool& horizontalAlign, const Qt::AlignmentFlag& alignment)
{
    if (activePage())
        activePage()->changeSelectedGrpoupTextAlignPropperty(horizontalAlign, alignment);
}

void ReportDesignWidget::setBorders(const BaseDesignIntf::BorderLines& borders)
{
    if (activePage())
        activePage()->setBorders(borders);
}

void ReportDesignWidget::setBordersExt(
    const BaseDesignIntf::BorderLines& border,
    const double borderWidth,
    const LimeReport::BaseDesignIntf::BorderStyle style,
    const QString color
){
    if (activePage())
        activePage()->setBordersExt(border, borderWidth, style, color);
}

void ReportDesignWidget::prepareReport()
{
    m_report->scriptContext()->setInitScript(m_scriptEditor->toPlainText());
    report()->clearSelection();
}

void ReportDesignWidget::initThemeIfExist(const QString &themeName, const QString &path)
{
    QFile theme(path);
    if (theme.exists()){
        theme.open(QIODevice::ReadOnly);
        QString styleSheet = theme.readAll();
        m_themes.insert(themeName, styleSheet);
    }
}

void ReportDesignWidget::previewReport()
{
    if (report()->isBusy()) return;
    prepareReport();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    updateDialogs();
#endif
    bool showProgressDialog = report()->isShowProgressDialog();
    report()->setShowProgressDialog(false);
    report()->previewReport();
    report()->setShowProgressDialog(showProgressDialog);
}

void ReportDesignWidget::printReport()
{
    if (report()->isBusy()) return;
    prepareReport();
#ifdef HAVE_QTDESIGNER_INTEGRATION
    updateDialogs();
#endif
    setCursor(Qt::WaitCursor);
    report()->printReport();
    setCursor(Qt::ArrowCursor);
}

void ReportDesignWidget::addPage()
{
    PageDesignIntf* page = m_report->appendPage("page"+QString::number(m_report->pageCount()+1));
    connectPage(page);
    PageView* view = createPageView(page);
    int index = m_report->pageCount()-1;
    m_tabWidget->insertTab(index, view, QIcon(), page->pageItem()->objectName());
    m_tabWidget->setTabWhatsThis(index, "page");
    m_tabWidget->setCurrentIndex(index);
    applyUseGrid();
    emit pageAdded(page);
}

void ReportDesignWidget::deleteCurrentPage()
{
    if (m_report->pageCount()>1){
        QGraphicsView* view = dynamic_cast<QGraphicsView*>(m_tabWidget->currentWidget());
        if (view){
            PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(view->scene());
            if (page){
                if (m_report->deletePage(page)){
                    int index = m_tabWidget->currentIndex();
                    m_tabWidget->removeTab(m_tabWidget->currentIndex());
                    if (index>0) m_tabWidget->setCurrentIndex(index-1);
                    emit pageDeleted();
                }
            }
        }
    }
}

void ReportDesignWidget::editSetting()
{
    SettingDialog setting(this);
    setting.setSettings(m_settings);
    setting.setVerticalGridStep(m_verticalGridStep);
    setting.setHorizontalGridStep(m_horizontalGridStep);
    setting.setDefaultFont(m_defaultFont);
    setting.setSuppressAbsentFieldsAndVarsWarnings(m_report->suppressFieldAndVarError());

    QStringList themes;
    themes.append(QObject::tr("Default"));
    foreach(QString theme, m_themes.keys())
        if (!themes.contains(QObject::tr(theme.toLatin1())))
            themes.append(QObject::tr(theme.toLatin1()));

    setting.setDesignerThemes(themes, QObject::tr(m_theme.toLatin1()));
    setting.setDesignerLanguages(m_report->designerLanguages(), m_report->currentDesignerLanguage());

    QList<QString> unitTypes;
    unitTypes << QObject::tr("Millimeters") << QObject::tr("Inches");
    setting.setDesignerUnites(unitTypes,
                              m_defaultUnits == BaseDesignIntf::Millimeters ?
                                  QObject::tr("Millimeters")  :
                                  QObject::tr("Inches"));

    if (setting.exec()){
        m_horizontalGridStep = setting.horizontalGridStep();
        m_verticalGridStep = setting.verticalGridStep();
        m_defaultFont = setting.defaultFont();
        if (setting.reportUnits().compare(QObject::tr("Millimeters")) == 0)
            m_defaultUnits = BaseDesignIntf::Millimeters;
        else {
            m_defaultUnits = BaseDesignIntf::Inches;
        }
        if (m_localToEng.contains(setting.theme())){
            m_theme =  m_localToEng.value(setting.theme());
        } else {
            m_theme = "Default";
        }
        m_report->setSuppressFieldAndVarError(setting.suppressAbsentFieldsAndVarsWarnings());
        if (m_report->currentDesignerLanguage() != setting.designerLanguage() ){
            m_report->setCurrentDesignerLanguage(setting.designerLanguage());
        }
        applySettings();
    }
}

void ReportDesignWidget::applyUseGrid()
{
    int hGridStep = m_useGrid ? m_horizontalGridStep : Const::DEFAULT_GRID_STEP;
    int vGridStep = m_useGrid ? m_verticalGridStep : Const::DEFAULT_GRID_STEP;
    for(int i = 0; i < m_report->pageCount(); ++i){
       m_report->pageAt(i)->setVerticalGridStep(hGridStep);
       m_report->pageAt(i)->setHorizontalGridStep(vGridStep);
    }
}

void ReportDesignWidget::setUseGrid(bool value)
{
    m_useGrid = value;
    applyUseGrid();
}

bool ReportDesignWidget::isCanUndo()
{
    if (activePage())
        return activePage()->isCanUndo();
    return false;
}

bool ReportDesignWidget::isCanRedo()
{
    if (activePage())
        return activePage()->isCanRedo();
    return false;
}

void ReportDesignWidget::slotSelectionChanged()
{
    QGraphicsScene* page=dynamic_cast<QGraphicsScene*>(sender());
    if (page){
        if (page->selectedItems().count()==1){
            BaseDesignIntf* item = dynamic_cast<BaseDesignIntf*>(page->selectedItems().at(0));
            if (item)
              emit (itemSelected(item));
        }
        else if (page->selectedItems().count()>1){
            emit (multiItemSelected());
        }
    }
}

DataSourceManager* ReportDesignWidget::dataManager()
{
    return m_report->dataManager();
}

ScriptEngineManager* ReportDesignWidget::scriptManager()
{
    return m_report->scriptManager();
}

ScriptEngineContext*ReportDesignWidget::scriptContext()
{
    return m_report->scriptContext();
}

void ReportDesignWidget::slotPagesLoadFinished()
{
    applySettings();
    //setActivePage(m_report->pageAt(0));
    emit loadFinished();
}

void ReportDesignWidget::slotDialogDeleted(QString dialogName)
{
    for (int i = 0; i<m_tabWidget->count(); ++i ){
        if (m_tabWidget->tabText(i).compare(dialogName) == 0){
            delete m_tabWidget->widget(i);
            break;
        }
    }
}

void ReportDesignWidget::lockSelectedItems()
{
    if (activePage())
        activePage()->lockSelectedItems();
}

void ReportDesignWidget::unlockSelectedItems()
{
    if (activePage())
        activePage()->unlockSelectedItems();
}

void ReportDesignWidget::selectOneLevelItems()
{
    if (activePage())
        activePage()->selectOneLevelItems();
}

void ReportDesignWidget::slotDatasourceCollectionLoaded(const QString & /*collectionName*/)
{
}

void ReportDesignWidget::slotSceneRectChanged(QRectF)
{
    if (activeView()) activeView()->centerOn(0,0);
}

void ReportDesignWidget::slotCurrentTabChanged(int index)
{
    QGraphicsView* view = dynamic_cast<QGraphicsView*>(m_tabWidget->widget(index));
    if (view) {
        if (view->scene()){
            //foreach (QGraphicsItem* item, view->scene()->selectedItems()) item->setSelected(false);
            view->scene()->clearSelection();
        }
        m_zoomer->setView(view);
    }
#ifdef HAVE_QTDESIGNER_INTEGRATION
    if (activeTabType() == Dialog){
        m_dialogDesignerManager->setActiveEditor(m_tabWidget->widget(index));
    }
    updateDialogs();
#endif
    if (activeTabType() == Translations){
        m_traslationEditor->setReportEngine(dynamic_cast<ITranslationContainer*>(report()));
    }

    if (activeTabType() == Script){
        m_scriptEditor->initCompleter();
        m_scriptEditor->setFocus();
    }

    emit activePageChanged();

    if (view) view->centerOn(0,0);
}

void ReportDesignWidget::slotReportLoaded()
{
    QByteArray editorState = m_scriptEditor->saveState();
    createTabs();
    m_scriptEditor->setPlainText(m_report->scriptContext()->initScript());
    m_scriptEditor->restoreState(editorState);
    emit loadFinished();
    m_dialogChanged = false;
}

void ReportDesignWidget::slotScriptTextChanged()
{
    m_report->scriptContext()->setInitScript(m_scriptEditor->toPlainText());
}

void ReportDesignWidget::slotItemPropertyObjectNameChanged(const QString& oldName, const QString& newName)
{
    PageDesignIntf* page = qobject_cast<PageDesignIntf*>(sender());
    if (page){
        ITranslationContainer* tc = dynamic_cast<ITranslationContainer*>(report());
        for (int i = 0; i < tc->translations()->values().count(); ++i){
            PageTranslation* pt = tc->translations()->values().at(i)->findPageTranslation(page->pageItem()->objectName());
            if (pt) pt->renameItem(oldName, newName);
        }
    }
}

#ifdef HAVE_QTDESIGNER_INTEGRATION

void ReportDesignWidget::addNewDialog()
{
    QFile templateUi(":/templates/templates/Dialog.ui");
    templateUi.open(QIODevice::ReadOnly|QIODevice::Text);
    QString templateStr = templateUi.readAll();
    QString dialogName = m_report->scriptContext()->getNewDialogName();
    templateStr.replace("$ClassName$", dialogName);
    m_report->scriptContext()->addDialog(dialogName,templateStr.toUtf8());
    createNewDialogTab(dialogName, templateStr.toUtf8());
}

void ReportDesignWidget::slotDialogChanged(QString )
{
    m_dialogChanged = true;
}

void ReportDesignWidget::slotDialogNameChanged(QString oldName, QString newName)
{
    for (int i = 0; i < m_tabWidget->count(); ++i){
        if (m_tabWidget->tabText(i).compare(oldName) == 0)
            m_tabWidget->setTabText(i, newName);
    }
    m_report->scriptContext()->changeDialogName(oldName, newName);
}

#endif

void ReportDesignWidget::slotPagePropertyObjectNameChanged(const QString &oldValue, const QString &newValue)
{
    ITranslationContainer* tc = dynamic_cast<ITranslationContainer*>(report());
    foreach(ReportTranslation* translation, tc->translations()->values()){
        translation->renamePage(oldValue, newValue);
    }

    for (int i = 0; i < m_tabWidget->count(); ++i ){
        if (m_tabWidget->tabText(i).compare(oldValue) == 0){
            QGraphicsView* view = dynamic_cast<QGraphicsView*>(m_tabWidget->widget(i));
            if (view){
                PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(view->scene());
                if (page->pageItem() == sender())
                    m_tabWidget->setTabText(i, newValue);
            }
        }
    }
}

void ReportDesignWidget::slotTabMoved(int from, int to)
{
    Q_UNUSED(from)
    Q_UNUSED(to)

    QList<PageDesignIntf*> pages;

    for ( int i = 0; i < m_tabWidget->tabBar()->count(); ++i){
        QGraphicsView* view = dynamic_cast<QGraphicsView*>(m_tabWidget->widget(i));
        if (view){
            PageDesignIntf* page = dynamic_cast<PageDesignIntf*>(view->scene());
            if (page){
                pages.append(page);
            }
        }
    }

    m_report->reorderPages(pages);

}

bool ReportDesignWidget::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Wheel){
        QWheelEvent* we = dynamic_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers()==Qt::ControlModifier){
#if QT_VERSION < QT_VERSION_CHECK(5,12,3)
            if(we->delta()<0) scale(1.2,1.2);
            else scale(1/1.2,1/1.2);
#else
            if(we->pixelDelta().x()<0) scale(1.2,1.2);
            else scale(1/1.2,1/1.2);
#endif
        }
    }
    return QWidget::eventFilter(target,event);
}

void ReportDesignWidget::clear()
{
    m_report->clearReport();
    m_tabWidget->clear();
    m_report->setReportFileName("");
    m_report->scriptContext()->setInitScript("");
    m_scriptEditor->setPlainText("");
}

void PageView::setPageItem(PageItemDesignIntf *pageItem)
{
    if (!pageItem) return;
    m_pageItem = pageItem;
    if (!m_horizontalRuller){
        m_horizontalRuller = new Ruler(Ruler::Horizontal, this);
        m_horizontalRuller->setPage(pageItem);
    }
    if (!m_verticalRuller){
        m_verticalRuller = new Ruler(Ruler::Vertical, this);
        m_verticalRuller->setPage(pageItem);
    }
}

bool PageView::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
        if (m_horizontalRuller && m_verticalRuller){
            m_horizontalRuller->setMousePos(dynamic_cast<QMouseEvent*>(event)->pos());
            m_verticalRuller->setMousePos(dynamic_cast<QMouseEvent*>(event)->pos());
            m_horizontalRuller->update();
            m_verticalRuller->update();
        }
        break;
    //case QEvent::Resize:
    case QEvent::Paint:
        if (m_horizontalRuller && m_verticalRuller){
            int x = mapFromScene(m_pageItem->boundingRect().x(),m_pageItem->boundingRect().y()).x();
            int y = mapFromScene(m_pageItem->boundingRect().x(),m_pageItem->boundingRect().y()).y();
            int width = mapFromScene(m_pageItem->boundingRect().bottomRight().x(),m_pageItem->boundingRect().bottomRight().y()).x();
            int height = mapFromScene(m_pageItem->boundingRect().bottomRight().x(),m_pageItem->boundingRect().bottomRight().y()).y();

            x = x < 0 ? 0 : x;
            y = y < 0 ? 0 : y;

            m_horizontalRuller->setGeometry(x+20, 0, (width-x), 20);
            m_verticalRuller->setGeometry(0, y+20, 20, (height - y));
            m_verticalRuller->update();
            m_horizontalRuller->update();

        }
        break;
    default:
        break;
    }

    return QGraphicsView::viewportEvent(event);
}

void Ruler::setPage(PageItemDesignIntf *page)
{
    m_page = page;

}

void Ruler::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setBrush(palette().window());
    painter.setPen(Qt::NoPen);
    painter.drawRect(event->rect());
//    painter.setPen(palette().windowText().color());

    if (m_page){
        qreal rulerWidth = m_page->geometry().width() / m_page->unitFactor();
        qreal rulerHeight = m_page->geometry().height() / m_page->unitFactor();

        QGraphicsView* view = qobject_cast<QGraphicsView*>(parent());

        int hStartPos = view->mapFromScene(0,0).x();
        int vStartPos = view->mapFromScene(0,0).y();

        QFont font = painter.font();
        font.setPointSize(7);
        painter.setFont(font);

        switch (m_type) {
        case Horizontal:
            painter.setPen(Qt::NoPen);

            if (isColorDark(palette().window().color()))
                painter.setBrush(QColor("#64893d"));
            else
                painter.setBrush(QColor("#b5da91"));

            drawItemWithChildren(&painter, m_page);
            painter.setPen(palette().windowText().color());

            for (int i = 0; i < rulerWidth / 10; ++i){
                int hs10 = view->mapFromScene(QPointF(m_page->geometry().topLeft().x() + i * 10 * m_page->unitFactor(), 0)).x();
                int hs5 = view->mapFromScene(QPointF(m_page->geometry().topLeft().x() + i * 10 * m_page->unitFactor() + 5 * m_page->unitFactor(), 0)).x();
                if (hs10 > 0){
                    if (hStartPos > 0){
                        hs10 -= hStartPos;
                        hs5  -= hStartPos;
                    }
                    painter.drawLine(hs10, 15, hs10, 20);
                    painter.drawLine(hs5, 10, hs5, 20);
                    if ( i > 0)
                        painter.drawText(QPoint(hs10 - (painter.fontMetrics().boundingRect(QString::number(i)).width()/2), 12),
                                         QString::number(i));
                }
            }
            painter.setPen(palette().windowText().color());
            painter.drawLine(m_mousePos.x() - (hStartPos > 0 ? hStartPos : 0) , 0,
                             m_mousePos.x() - (hStartPos > 0 ? hStartPos : 0) , 20);
            break;
        case Vertical:
            painter.setPen(Qt::NoPen);

            if (isColorDark(palette().window().color()))
                painter.setBrush(QColor("#64893d"));
            else
                painter.setBrush(QColor("#b5da91"));

            drawItemWithChildren(&painter, m_page);
            painter.setPen(palette().windowText().color());
            for (int i = 0; i < rulerHeight / 10; ++i){
                int vs10 = view->mapFromScene(QPointF(0, m_page->geometry().topLeft().y()+i * 10 * m_page->unitFactor())).y();
                int vs5  = view->mapFromScene(QPointF(0, m_page->geometry().topLeft().y()+i * 10 * m_page->unitFactor() + 5 * m_page->unitFactor())).y();
                if (vs10 > 0){
                    if (vStartPos > 0){
                        vs10 -= vStartPos;
                        vs5 -= vStartPos;
                    }
                    painter.drawLine(15, vs10, 20, vs10);
                    if ( i > 0 )
                        painter.drawText(QPoint( (15 - painter.fontMetrics().boundingRect(QString::number(i)).width()) / 2 ,
                                                 vs10 + (painter.fontMetrics().height()/2)), QString::number(i));
                    painter.drawLine(10, vs5, 20, vs5);
                }
            }
            painter.setPen(palette().windowText().color());
            painter.drawLine(0, m_mousePos.y() - (vStartPos > 0 ? vStartPos : 0),
                             20, m_mousePos.y() - (vStartPos > 0 ? vStartPos : 0));
            break;
        }
    }
}

void Ruler::drawItemWithChildren(QPainter* painter, BaseDesignIntf *item)
{
    foreach(BaseDesignIntf* child, item->childBaseItems()){
        if (!child->childBaseItems().isEmpty())
            drawItemWithChildren(painter, child);
        else drawItem(painter, child);

    }
    drawItem(painter, item);
}

void Ruler::drawItem(QPainter* painter, BaseDesignIntf *item)
{
    if (!item->isSelected()) return;

    QGraphicsView* view = qobject_cast<QGraphicsView*>(parent());
    int hStartPos = view->mapFromScene(0,0).x();
    int vStartPos = view->mapFromScene(0,0).y();

    int itemWidth = view->mapFromScene(item->mapToScene(item->geometry().width(),0).x() - item->mapToScene(0,0).x(), 0).x() - hStartPos;
    int itemHeight = view->mapFromScene(0, item->mapToScene(0, item->geometry().height()).y() - item->mapToScene(0,0).y()).y() - vStartPos;

    switch (m_type) {
    case Horizontal:
        if (item->isSelected())
         painter->drawRect(view->mapFromScene(item->mapToScene(0,0)).x() - (hStartPos > 0 ? hStartPos : 0)  , 0,
                          itemWidth, 20);
        break;
    case Vertical:
        if (item->isSelected())
            painter->drawRect(0, view->mapFromScene(item->mapToScene(0, 0)).y() - (vStartPos > 0 ? vStartPos : 0),
                             20, itemHeight);
        break;
    }
}

}



