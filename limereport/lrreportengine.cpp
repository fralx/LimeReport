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
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileSystemWatcher>

#include "time.h"

#include "lrreportengine_p.h"
#include "lrreportengine.h"

#include "lrpagedesignintf.h"
#include "lrdatasourcemanager.h"
#include "lrdatabrowser.h"
#include "lrreportdesignwindow.h"

#include "serializators/lrxmlwriter.h"
#include "serializators/lrxmlreader.h"
#include "lrreportrender.h"
#include "lrpreviewreportwindow.h"
#include "lrpreviewreportwidget.h"
#include "lrpreviewreportwidget_p.h"
#ifdef HAVE_STATIC_BUILD
#include "lrfactoryinitializer.h"
#endif
namespace LimeReport{

QSettings* ReportEngine::m_settings = 0;

ReportEnginePrivate::ReportEnginePrivate(QObject *parent) :
    QObject(parent), m_fileName(""), m_settings(0), m_ownedSettings(false),
    m_printer(new QPrinter(QPrinter::HighResolution)), m_printerSelected(false),
    m_showProgressDialog(true), m_reportName(""), m_activePreview(0),
    m_previewWindowIcon(":/report/images/logo32"), m_previewWindowTitle(tr("Preview")),
    m_reportRendering(false), m_resultIsEditable(true), m_passPhrase("HjccbzHjlbyfCkjy"),
    m_fileWatcher( new QFileSystemWatcher( this ) )
{
#ifdef HAVE_STATIC_BUILD
    initResources();
    initReportItems();
    initObjectInspectorProperties();
    initSerializators();
#endif
    m_datasources = new DataSourceManager(this);
    m_datasources->setReportSettings(&m_reportSettings);
    scriptManager()->setDataManager(m_datasources);
    m_scriptEngineContext = new ScriptEngineContext(this);
    m_datasources->setObjectName("datasources");
    connect(m_datasources,SIGNAL(loadCollectionFinished(QString)),this,SLOT(slotDataSourceCollectionLoaded(QString)));
    connect(m_fileWatcher,SIGNAL(fileChanged(const QString &)),this,SLOT(slotLoadFromFile(const QString &)));
}

ReportEnginePrivate::~ReportEnginePrivate()
{
    if (m_designerWindow) {
        m_designerWindow->close();
    }
    if (m_activePreview){
        m_activePreview->close();
    }
    foreach(PageDesignIntf* page,m_pages) delete page;
    m_pages.clear();
    if (m_ownedSettings&&m_settings) delete m_settings;
}

QObject* ReportEnginePrivate::createElement(const QString &, const QString &)
{
    return appendPage();
}

QObject *ReportEnginePrivate::elementAt(const QString &, int index)
{
    return pageAt(index);
}

PageDesignIntf *ReportEnginePrivate::createPage(const QString &pageName)
{
    PageDesignIntf* page =new PageDesignIntf();
    page->setObjectName(pageName);
    page->pageItem()->setObjectName("Report"+pageName);
    page->setReportEditor(this);
    page->setReportSettings(&m_reportSettings);
    return page;
}

PageDesignIntf *ReportEnginePrivate::appendPage(const QString &pageName)
{
    PageDesignIntf* page = createPage(pageName);
    m_pages.append(page);
    return page;
}

bool ReportEnginePrivate::deletePage(PageDesignIntf *page){
    QList<PageDesignIntf*>::iterator it = m_pages.begin();
    while (it != m_pages.end()){
        if (*it == page) {
            it = m_pages.erase(it);
            return true;
        } else ++it;
    }
    return false;
}

PageDesignIntf *ReportEnginePrivate::createPreviewPage()
{
    return createPage();
}

int ReportEnginePrivate::elementsCount(const QString &)
{
    return m_pages.count();
}

void ReportEnginePrivate::collectionLoadFinished(const QString &)
{
    foreach (PageDesignIntf* page, m_pages) {
        page->setReportEditor(this);
        page->setReportSettings(&m_reportSettings);
        page->setSceneRect(-Const::SCENE_MARGIN,-Const::SCENE_MARGIN,
                           page->pageItem()->width()+Const::SCENE_MARGIN*2,
                           page->pageItem()->height()+Const::SCENE_MARGIN*2);
    }
    emit pagesLoadFinished();
}

void ReportEnginePrivate::saveError(QString message)
{
    m_lastError = message;
}

void ReportEnginePrivate::showError(QString message)
{
    QMessageBox::critical(0,tr("Error"),message);
}

void ReportEnginePrivate::slotDataSourceCollectionLoaded(const QString &collectionName)
{
    emit datasourceCollectionLoadFinished(collectionName);
}

void ReportEnginePrivate::slotPreviewWindowDestroyed(QObject* window)
{
    if (m_activePreview == window){
        m_activePreview = 0;
    }
}

void ReportEnginePrivate::clearReport()
{
    foreach(PageDesignIntf* page,m_pages) delete page;
    m_pages.clear();
    m_datasources->clear(DataSourceManager::Owned);
    m_fileName="";
    m_scriptEngineContext->clear();
    m_reportSettings.setDefaultValues();
    emit cleared();
}

void ReportEnginePrivate::printReport(ItemsReaderIntf::Ptr reader, QPrinter& printer)
{
    LimeReport::PageDesignIntf renderPage;
    renderPage.setItemMode(PrintMode);
    if (reader->first()){
        reader->readItem(renderPage.pageItem());
        printer.setFullPage(renderPage.pageItem()->fullPage());
        printer.setOrientation((QPrinter::Orientation)renderPage.pageItem()->pageOrientation());
        renderPage.setSceneRect(renderPage.pageItem()->mapToScene(renderPage.pageItem()->rect()).boundingRect());

        if (renderPage.pageItem()->pageSize()==PageItemDesignIntf::Custom){
            QSizeF pageSize = (renderPage.pageItem()->pageOrientation()==PageItemDesignIntf::Landscape)?
                        QSizeF(renderPage.pageItem()->sizeMM().height(),renderPage.pageItem()->sizeMM().width()):
                        renderPage.pageItem()->sizeMM();
            printer.setPaperSize(pageSize,QPrinter::Millimeter);
        } else {
            printer.setPaperSize((QPrinter::PageSize)renderPage.pageItem()->pageSize());
        }

        QPainter painter(&printer);
        renderPage.render(&painter);

        while (reader->next()){
            printer.newPage();
            renderPage.removeAllItems();
            reader->readItem(renderPage.pageItem());
            renderPage.setSceneRect(renderPage.pageItem()->mapToScene(renderPage.pageItem()->rect()).boundingRect());
            renderPage.render(&painter);
        }
    }
}

void ReportEnginePrivate::printReport(ReportPages pages, QPrinter &printer)
{
    LimeReport::PageDesignIntf renderPage;
    renderPage.setItemMode(PrintMode);
    QPainter* painter=0;

    bool isFirst = true;
    int currenPage = 1;
    foreach(PageItemDesignIntf::Ptr page, pages){

        if (
                (printer.printRange() == QPrinter::AllPages) ||
                (   (printer.printRange()==QPrinter::PageRange) &&
                    (currenPage>=printer.fromPage()) &&
                    (currenPage<=printer.toPage())
                )
           )
        {

            QPointF pagePos = page->pos();
            page->setPos(0,0);
            renderPage.setPageItem(page);
            renderPage.setSceneRect(renderPage.pageItem()->mapToScene(renderPage.pageItem()->rect()).boundingRect());
            if (renderPage.pageItem()->oldPrintMode()){
                printer.setPageMargins(renderPage.pageItem()->leftMargin(),
                                      renderPage.pageItem()->topMargin(),
                                      renderPage.pageItem()->rightMargin(),
                                      renderPage.pageItem()->bottomMargin(),
                                      QPrinter::Millimeter);
                printer.setOrientation((QPrinter::Orientation)renderPage.pageItem()->pageOrientation());
                QSizeF pageSize = (renderPage.pageItem()->pageOrientation()==PageItemDesignIntf::Landscape)?
                           QSizeF(renderPage.pageItem()->sizeMM().height(),renderPage.pageItem()->sizeMM().width()):
                           renderPage.pageItem()->sizeMM();
                printer.setPaperSize(pageSize,QPrinter::Millimeter);
            } else {
                printer.setFullPage(renderPage.pageItem()->fullPage());
                printer.setOrientation((QPrinter::Orientation)renderPage.pageItem()->pageOrientation());
                if (renderPage.pageItem()->pageSize()==PageItemDesignIntf::Custom){
                    QSizeF pageSize = (renderPage.pageItem()->pageOrientation()==PageItemDesignIntf::Landscape)?
                                QSizeF(renderPage.pageItem()->sizeMM().height(),renderPage.pageItem()->sizeMM().width()):
                                renderPage.pageItem()->sizeMM();
                    printer.setPaperSize(pageSize,QPrinter::Millimeter);
                } else {
                    printer.setPaperSize((QPrinter::PageSize)renderPage.pageItem()->pageSize());
                }
            }

            if (!isFirst){
                printer.newPage();
            } else {
                isFirst=false;
                painter = new QPainter(&printer);
            }

            renderPage.render(painter);
            page->setPos(pagePos);
        }

        currenPage++;
    }
    delete painter;
}

bool ReportEnginePrivate::printReport(QPrinter* printer)
{
    if (!printer&&!m_printerSelected){
        QPrintDialog dialog(m_printer.data(),QApplication::activeWindow());
        m_printerSelected = dialog.exec()!=QDialog::Rejected;
    }
    if (!printer&&!m_printerSelected) return false;

    printer =(printer)?printer:m_printer.data();
    if (printer&&printer->isValid()){
        try{
			dataManager()->setDesignTime(false);
            ReportPages pages = renderToPages();
            dataManager()->setDesignTime(true);
            if (pages.count()>0){
                printReport(pages,*printer);
            }
        } catch(ReportError &exception){
            saveError(exception.what());
        }
        return true;
    } else return false;
}

bool ReportEnginePrivate::printPages(ReportPages pages, QPrinter *printer)
{

    if (!printer&&!m_printerSelected){
        QPrintDialog dialog(m_printer.data(),QApplication::activeWindow());
        m_printerSelected = dialog.exec()!=QDialog::Rejected;
    }
    if (!printer&&!m_printerSelected) return false;

    printer =(printer)?printer:m_printer.data();
    if (printer&&printer->isValid()){
        try{
            if (pages.count()>0){
                printReport(
                    pages,
                    *printer
                );
            }
        } catch(ReportError &exception){
            saveError(exception.what());
        }
        return true;
    } else return false;
}

void ReportEnginePrivate::printToFile(const QString &fileName)
{
    if (!fileName.isEmpty()){
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)){
            QTextStream out(&file);
            try {
                dataManager()->setDesignTime(false);
                out<<renderToString();
                dataManager()->setDesignTime(true);
            }  catch( ReportError &exception){
                saveError(exception.what());
            }
        }
        file.close();
    }
}

bool ReportEnginePrivate::printToPDF(const QString &fileName)
{
    if (!fileName.isEmpty()){
        QFileInfo fi(fileName);
        QString fn = fileName;
        if (fi.suffix().isEmpty())
            fn+=".pdf";
        QPrinter printer;
        printer.setOutputFileName(fn);
        printer.setOutputFormat(QPrinter::PdfFormat);
        return printReport(&printer);
    }
    return false;
}

void ReportEnginePrivate::previewReport(PreviewHints hints)
{ 
//    QTime start = QTime::currentTime();
    try{
        dataManager()->setDesignTime(false);
        ReportPages pages = renderToPages();
        dataManager()->setDesignTime(true);
        if (pages.count()>0){
            PreviewReportWindow* w = new PreviewReportWindow(this,0,settings());
            w->setWindowFlags(Qt::Dialog|Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint| Qt::WindowMinMaxButtonsHint);
            w->setAttribute(Qt::WA_DeleteOnClose,true);
            w->setWindowModality(Qt::ApplicationModal);
            //w->setWindowIcon(QIcon(":/report/images/main.ico"));
            w->setWindowIcon(m_previewWindowIcon);
            w->setWindowTitle(m_previewWindowTitle);
            w->setSettings(settings());
            w->setPages(pages);
            w->setLayoutDirection(m_previewLayoutDirection);
            if (!dataManager()->errorsList().isEmpty()){
                w->setErrorMessages(dataManager()->errorsList());
            }

            if (!hints.testFlag(PreviewBarsUserSetting)){
                w->setMenuVisible(!hints.testFlag(HidePreviewMenuBar));
                w->setStatusBarVisible(!hints.testFlag(HidePreviewStatusBar));
                w->setToolBarVisible(!hints.testFlag(HidePreviewToolBar));
            }

            w->setHideResultEditButton(resultIsEditable());

            m_activePreview = w;
            connect(w,SIGNAL(destroyed(QObject*)), this, SLOT(slotPreviewWindowDestroyed(QObject*)));
            w->exec();
        }
    } catch (ReportError &exception){
        saveError(exception.what());
        showError(exception.what());
    }
}

PreviewReportWidget* ReportEnginePrivate::createPreviewWidget(QWidget* parent){

    PreviewReportWidget* widget = new PreviewReportWidget(this, parent);
    try{
        dataManager()->setDesignTime(false);
        ReportPages pages = renderToPages();
        dataManager()->setDesignTime(true);
        if (pages.count()>0)
            widget->d_ptr->setPages(pages);
    } catch (ReportError &exception){
        saveError(exception.what());
        showError(exception.what());
    }
    return widget;
}

PageDesignIntf* ReportEnginePrivate::createPreviewScene(QObject* parent){
    PageDesignIntf* result = 0;
    try {
        ReportPages pages = renderToPages();
        result = new PageDesignIntf(parent);
        result->setPageItems(pages);
    } catch (ReportError &exception){
        saveError(exception.what());
        showError(exception.what());
    }
    return result;
}

void ReportEnginePrivate::emitSaveReport()
{
    emit onSave();
}

bool ReportEnginePrivate::emitLoadReport()
{
    bool result = false;
    emit onLoad(result);
    return result;
}

void ReportEnginePrivate::emitSaveFinished()
{
    emit saveFinished();
}

bool ReportEnginePrivate::isSaved()
{
    foreach (PageDesignIntf* page, m_pages) {
        if (!page->isSaved()) return false;
    }
    return true;
}

void ReportEnginePrivate::setCurrentReportsDir(const QString &dirName)
{
    if (QDir(dirName).exists())
        m_reportsDir = dirName;
}

bool ReportEnginePrivate::slotLoadFromFile(const QString &fileName)
{
    PreviewReportWindow  *currentPreview = qobject_cast<PreviewReportWindow *>(m_activePreview);
   
    if (!QFile::exists(fileName))
    {
       if ( hasActivePreview() )
       {          
          QMessageBox::information( NULL,
                                    tr( "Report File Change" ),
                                    tr( "The report file \"%1\" has changed names or been deleted.\n\nThis preview is no longer valid." ).arg( fileName )
                                    );
          
          clearReport();
          
          currentPreview->close();
       }
       
       return false;
    }

    clearReport();

    ItemsReaderIntf::Ptr reader = FileXMLReader::create(fileName);
    reader->setPassPhrase(m_passPhrase);
    if (reader->first()){
        if (reader->readItem(this)){
            m_fileName=fileName;
            QFileInfo fi(fileName);
            m_reportName = fi.fileName();

            QString dbSettingFileName = fi.absolutePath()+"/"+fi.baseName()+".db";
            if (QFile::exists(dbSettingFileName)){
                QSettings dbcredentals(dbSettingFileName, QSettings::IniFormat);
                foreach (ConnectionDesc* connection, dataManager()->conections()) {
                    if (!connection->keepDBCredentials()){
                        dbcredentals.beginGroup(connection->name());
                        connection->setUserName(dbcredentals.value("user").toString());
                        connection->setPassword(dbcredentals.value("password").toString());
                        dbcredentals.endGroup();
                    }
                }
            }

            dataManager()->connectAutoConnections();

            if ( hasActivePreview() )
            {
               currentPreview->reloadPreview();
            }
            return true;
        };
    }
    m_lastError = reader->lastError();
    return false;
}

void ReportEnginePrivate::cancelRender()
{
    if (m_reportRender)
        m_reportRender->cancelRender();
    m_reportRendering = false;
}

PageDesignIntf* ReportEngine::createPreviewScene(QObject* parent){
    Q_D(ReportEngine);
    return d->createPreviewScene(parent);
}

void ReportEnginePrivate::designReport()
{
    if (!m_designerWindow) {
            m_designerWindow = new LimeReport::ReportDesignWindow(this,QApplication::activeWindow(),settings());
            m_designerWindow->setAttribute(Qt::WA_DeleteOnClose,true);
            m_designerWindow->setWindowIcon(QIcon(":report/images/logo32"));
            m_designerWindow->setShowProgressDialog(m_showProgressDialog);
     }

#ifdef Q_OS_WIN    
    m_designerWindow->setWindowModality(Qt::ApplicationModal);
#endif
    if (QApplication::activeWindow()==0){
        m_designerWindow->show();;
    } else {
        m_designerWindow->showModal();
    }
}

void ReportEnginePrivate::setSettings(QSettings* value)
{
    if (value){
        if (m_ownedSettings&&m_settings)
            delete m_settings;
        m_settings = value;
        m_ownedSettings = false;
    }
}

QSettings*ReportEnginePrivate::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QApplication::applicationName());
        m_ownedSettings=true;
        return m_settings;
    }
}

bool ReportEnginePrivate::loadFromFile(const QString &fileName, bool autoLoadPreviewOnChange)
{
   // only watch one file at a time
   if ( !m_fileWatcher->files().isEmpty() )
   {
      m_fileWatcher->removePaths( m_fileWatcher->files() );
   }

   if ( autoLoadPreviewOnChange )
   {
      m_fileWatcher->addPath( fileName );
   }

   return slotLoadFromFile( fileName );
}

bool ReportEnginePrivate::loadFromByteArray(QByteArray* data, const QString &name){
    clearReport();

    ItemsReaderIntf::Ptr reader = ByteArrayXMLReader::create(data);
    reader->setPassPhrase(m_passPhrase);
    if (reader->first()){
        if (reader->readItem(this)){
            m_fileName = "";
            m_reportName = name;
            return true;
        };
    }
    return false;
}

bool ReportEnginePrivate::loadFromString(const QString &report, const QString &name)
{
    clearReport();

    ItemsReaderIntf::Ptr reader = StringXMLreader::create(report);
    reader->setPassPhrase(m_passPhrase);
    if (reader->first()){
        if (reader->readItem(this)){
            m_fileName = "";
            m_reportName = name;
            return true;
        };
    }
    return false;
}

bool ReportEnginePrivate::saveToFile(const QString &fileName)
{
    if (fileName.isEmpty()) return false;
    QFileInfo fi(fileName);
    QString fn = fileName;
    if (fi.suffix().isEmpty())
        fn+=".lrxml";

    QString dbSettingFileName = fi.absolutePath()+"/"+fi.baseName()+".db";
    QSettings dbcredentals(dbSettingFileName, QSettings::IniFormat);

    foreach (ConnectionDesc* connection, dataManager()->conections()) {
        if (!connection->keepDBCredentials()){
            dbcredentals.beginGroup(connection->name());
            dbcredentals.setValue("user",connection->userName());
            dbcredentals.setValue("password",connection->password());
            dbcredentals.endGroup();
            connection->setPassword("");
            connection->setUserName("");
        }
    }

    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    writer->setPassPhrase(m_passPhrase);
    writer->putItem(this);
    m_fileName=fn;   
    bool saved = writer->saveToFile(fn);

    foreach (ConnectionDesc* connection, dataManager()->conections()) {
        if (!connection->keepDBCredentials()){
            dbcredentals.beginGroup(connection->name());
            connection->setUserName(dbcredentals.value("user").toString());
            connection->setPassword(dbcredentals.value("password").toString());
            dbcredentals.endGroup();
        }
    }

    if (saved){
        foreach(PageDesignIntf* page, m_pages){
            page->setToSaved();
        }
    }
    return saved;
}

QByteArray ReportEnginePrivate::saveToByteArray()
{
    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    writer->setPassPhrase(m_passPhrase);
    writer->putItem(this);
    QByteArray result = writer->saveToByteArray();
    if (!result.isEmpty()){
        foreach(PageDesignIntf* page, m_pages){
            page->setToSaved();
        }
    }
    return result;
}

QString ReportEnginePrivate::saveToString(){
    QScopedPointer< ItemsWriterIntf > writer(new XMLWriter());
    writer->setPassPhrase(m_passPhrase);
    writer->putItem(this);
    QString result = writer->saveToString();
    if (!result.isEmpty()){
        foreach(PageDesignIntf* page, m_pages){
            page->setToSaved();
        }
    }
    return result;
}

bool ReportEnginePrivate::isNeedToSave()
{
    foreach(PageDesignIntf* page, m_pages){
        if (page->isHasChanges()) return true;
    }
    return false;
}

bool ReportEnginePrivate::saveToFile()
{
    if (m_fileName.isEmpty()) return false;
    return saveToFile(m_fileName);
}

QString ReportEnginePrivate::renderToString()
{
    LimeReport::ReportRender render;
    dataManager()->connectAllDatabases();
    dataManager()->setDesignTime(false);
    if (m_pages.count()){
        render.setDatasources(dataManager());
        render.setScriptContext(scriptContext());
        return render.renderPageToString(m_pages.at(0));
    }else return QString();
}

Qt::LayoutDirection ReportEnginePrivate::previewLayoutDirection()
{
    return m_previewLayoutDirection;
}

void ReportEnginePrivate::setPreviewLayoutDirection(const Qt::LayoutDirection& layoutDirection)
{
    m_previewLayoutDirection = layoutDirection;
}

void ReportEnginePrivate::setPassPhrase(const QString &passPhrase)
{
    m_passPhrase = passPhrase;
}

void ReportEnginePrivate::reorderPages(const QList<PageDesignIntf *>& reorderedPages)
{
    m_pages.clear();
    foreach(PageDesignIntf* page, reorderedPages){
        m_pages.append(page);
    }
}

void ReportEnginePrivate::clearSelection()
{
    foreach (PageDesignIntf* page, m_pages) {
        foreach(QGraphicsItem* item, page->selectedItems()){
            item->setSelected(false);
        }
    }
}

bool ReportEnginePrivate::resultIsEditable() const
{
    return m_resultIsEditable;
}

void ReportEnginePrivate::setResultEditable(bool value)
{
    m_resultIsEditable = value;
}

bool ReportEnginePrivate::suppressFieldAndVarError() const
{
    return m_reportSettings.suppressAbsentFieldsAndVarsWarnings();
}

void ReportEnginePrivate::setSuppressFieldAndVarError(bool suppressFieldAndVarError)
{
    m_reportSettings.setSuppressAbsentFieldsAndVarsWarnings(suppressFieldAndVarError);
}

bool ReportEnginePrivate::isBusy()
{
    return m_reportRendering;
}

QString ReportEnginePrivate::previewWindowTitle() const
{
    return m_previewWindowTitle;
}

void ReportEnginePrivate::setPreviewWindowTitle(const QString &previewWindowTitle)
{
    m_previewWindowTitle = previewWindowTitle;
}

QIcon ReportEnginePrivate::previewWindowIcon() const
{
    return m_previewWindowIcon;
}

void ReportEnginePrivate::setPreviewWindowIcon(const QIcon &previewWindowIcon)
{
    m_previewWindowIcon = previewWindowIcon;
}

ReportPages ReportEnginePrivate::renderToPages()
{
    if (m_reportRendering) return ReportPages();
    m_reportRender = ReportRender::Ptr(new ReportRender);

    dataManager()->clearErrors();
    dataManager()->connectAllDatabases();
    dataManager()->setDesignTime(false);
    dataManager()->updateDatasourceModel();

    connect(m_reportRender.data(),SIGNAL(pageRendered(int)),
            this, SIGNAL(renderPageFinished(int)));
    if (m_pages.count()){
        ReportPages result;
        m_reportRendering = true;
        emit renderStarted();
        m_reportRender->setDatasources(dataManager());
        m_reportRender->setScriptContext(scriptContext());

        foreach(PageDesignIntf* page , m_pages){
        	m_pages.at(0)->setReportSettings(&m_reportSettings);
        	result.append(m_reportRender->renderPageToPages(page));
        }

        m_reportRender->secondRenderPass(result);
        emit renderFinished();
        m_reportRender.clear();
        m_reportRendering = false;
        return result;
    } else {
        return ReportPages();
    }
}

QString ReportEnginePrivate::lastError()
{
    return m_lastError;
}

ReportEngine::ReportEngine(QObject *parent)
    : QObject(parent), d_ptr(new ReportEnginePrivate())
{
    Q_D(ReportEngine);
    d->q_ptr=this;
    connect(d, SIGNAL(renderStarted()), this, SIGNAL(renderStarted()));
    connect(d, SIGNAL(renderPageFinished(int)),
            this, SIGNAL(renderPageFinished(int)));
    connect(d, SIGNAL(renderFinished()), this, SIGNAL(renderFinished()));
    connect(d, SIGNAL(onSave()), this, SIGNAL(onSave()));
    connect(d, SIGNAL(onLoad(bool&)), this, SIGNAL(onLoad(bool&)));
    connect(d, SIGNAL(saveFinished()), this, SIGNAL(saveFinished()));
}

ReportEngine::~ReportEngine()
{
    delete d_ptr;
}

bool ReportEngine::printReport(QPrinter *printer)
{
    Q_D(ReportEngine);
    return d->printReport(printer);
}

bool ReportEngine::printPages(ReportPages pages, QPrinter *printer){
    Q_D(ReportEngine);
    return d->printPages(pages,printer);
}

void ReportEngine::printToFile(const QString &fileName)
{
    Q_D(ReportEngine);
    d->printToFile(fileName);
}

bool ReportEngine::printToPDF(const QString &fileName)
{
    Q_D(ReportEngine);
    return d->printToPDF(fileName);
}

void ReportEngine::previewReport(PreviewHints hints)
{
    Q_D(ReportEngine);
    if (m_settings)
        d->setSettings(m_settings);
    d->previewReport(hints);
}

void ReportEngine::designReport()
{
    Q_D(ReportEngine);
    if (m_settings)
        d->setSettings(m_settings);
    d->designReport();
}

PreviewReportWidget* ReportEngine::createPreviewWidget(QWidget *parent)
{
    Q_D(ReportEngine);
    return d->createPreviewWidget(parent);
}

void ReportEngine::setPreviewWindowTitle(const QString &title)
{
    Q_D(ReportEngine);
    d->setPreviewWindowTitle(title);
}

void ReportEngine::setPreviewWindowIcon(const QIcon &icon)
{
    Q_D(ReportEngine);
    d->setPreviewWindowIcon(icon);
}

void ReportEngine::setResultEditable(bool value)
{
    Q_D(ReportEngine);
    d->setResultEditable(value);
}

bool ReportEngine::resultIsEditable()
{
    Q_D(ReportEngine);
    return d->resultIsEditable();
}

bool ReportEngine::isBusy()
{
    Q_D(ReportEngine);
    return d->isBusy();
}

void ReportEngine::setPassPharse(QString &passPharse)
{
    Q_D(ReportEngine);
    d->setPassPhrase(passPharse);
}

Qt::LayoutDirection ReportEngine::previewLayoutDirection()
{
    Q_D(ReportEngine);
    return d->previewLayoutDirection();
}

void ReportEngine::setPreviewLayoutDirection(const Qt::LayoutDirection& layoutDirection)
{
    Q_D(ReportEngine);
    return d->setPreviewLayoutDirection(layoutDirection);
}

void ReportEngine::setShowProgressDialog(bool value)
{
    Q_D(ReportEngine);
    d->setShowProgressDialog(value);
}

IDataSourceManager *ReportEngine::dataManager()
{
    Q_D(ReportEngine);
    return d->dataManagerIntf();
}

IScriptEngineManager* ReportEngine::scriptManager()
{
    Q_D(ReportEngine);
    return d->scriptManagerIntf();
}

bool ReportEngine::loadFromFile(const QString &fileName, bool autoLoadPreviewOnChange)
{
    Q_D(ReportEngine);
    return d->loadFromFile(fileName, autoLoadPreviewOnChange);
}

bool ReportEngine::loadFromByteArray(QByteArray* data){
    Q_D(ReportEngine);
    return d->loadFromByteArray(data);
}

bool ReportEngine::loadFromString(const QString &data)
{
    Q_D(ReportEngine);
    return d->loadFromString(data);
}

QString ReportEngine::reportFileName()
{
    Q_D(ReportEngine);
    return d->reportFileName();
}

void ReportEngine::setReportFileName(const QString &fileName)
{
    Q_D(ReportEngine);
    return d->setReportFileName(fileName);
}

bool ReportEngine::saveToFile()
{
    Q_D(ReportEngine);
    return d->saveToFile();
}

bool ReportEngine::saveToFile(const QString &fileName)
{
    Q_D(ReportEngine);
    return d->saveToFile(fileName);
}

QByteArray ReportEngine::saveToByteArray()
{
    Q_D(ReportEngine);
    return d->saveToByteArray();
}

QString ReportEngine::saveToString()
{
    Q_D(ReportEngine);
    return d->saveToString();
}

QString ReportEngine::lastError()
{
    Q_D(ReportEngine);
    return d->lastError();
}

void ReportEngine::setCurrentReportsDir(const QString &dirName)
{
    Q_D(ReportEngine);
    return d->setCurrentReportsDir(dirName);
}

void ReportEngine::setReportName(const QString &name)
{
    Q_D(ReportEngine);
    return d->setReportName(name);
}

QString ReportEngine::reportName()
{
    Q_D(ReportEngine);
    return d->reportName();
}

void ReportEngine::cancelRender()
{
    Q_D(ReportEngine);
    d->cancelRender();
}

ReportEngine::ReportEngine(ReportEnginePrivate &dd, QObject *parent)
    :QObject(parent),d_ptr(&dd)
{
    Q_D(ReportEngine);
    d->q_ptr=this;
    connect(d, SIGNAL(renderStarted()), this, SIGNAL(renderStarted()));
    connect(d, SIGNAL(renderPageFinished(int)),
            this, SIGNAL(renderPageFinished(int)));
    connect(d, SIGNAL(renderFinished()), this, SIGNAL(renderFinished()));
}



}
