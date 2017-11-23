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
#include <QPrinterInfo>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileSystemWatcher>

#include "time.h"

#include "limerender_p.h"
#include "limerender.h"

#include "lrpagedesignintf.h"
#include "lrdatasourcemanager.h"
#include "lrreportrender.h"
#include "serializators/lrxmlwriter.h"
#include "serializators/lrxmlreader.h"
#include "lrpreviewreportwindow.h"
#include "lrpreviewreportwidget.h"
#include "lrpreviewreportwidget_p.h"

#ifdef HAVE_STATIC_BUILD
#include "lrfactoryinitializer.h"
#endif
namespace LimeReport{

QSettings* LimeRender::m_settings = 0;

LimeRenderPrivate::LimeRenderPrivate(QObject *parent) :
    QObject(parent), m_fileName(""), m_settings(0), m_ownedSettings(false),
    m_printer(new QPrinter(QPrinter::HighResolution)), m_printerSelected(false),
    m_showProgressDialog(true), m_reportName(""), m_activePreview(0),
    m_previewWindowIcon(":/report/images/logo32"), m_previewWindowTitle(tr("Preview")),
    m_LimeRendering(false), m_resultIsEditable(true), m_passPhrase("HjccbzHjlbyfCkjy"),
    m_fileWatcher( new QFileSystemWatcher( this ) ), m_reportLanguage(QLocale::AnyLanguage)
{
#ifdef HAVE_STATIC_BUILD
    initResources();
    initReportItems();
    initObjectInspectorProperties();
    initSerializators();
#endif
    m_datasources = new DataSourceManager(this);
    m_datasources->setReportSettings(&m_reportSettings);
    m_scriptEngineContext = new ScriptEngineContext(this);

    ICallbackDatasource* tableOfContens = m_datasources->createCallbackDatasource("tableofcontens");
    connect(tableOfContens, SIGNAL(getCallbackData(LimeReport::CallbackInfo,QVariant&)),
            m_scriptEngineContext->tableOfContens(), SLOT(slotOneSlotDS(LimeReport::CallbackInfo,QVariant&)));

    m_datasources->setObjectName("datasources");
    connect(m_datasources,SIGNAL(loadCollectionFinished(QString)),this,SLOT(slotDataSourceCollectionLoaded(QString)));
    connect(m_fileWatcher,SIGNAL(fileChanged(const QString &)),this,SLOT(slotLoadFromFile(const QString &)));
}

LimeRenderPrivate::~LimeRenderPrivate()
{
    if (m_activePreview){
        m_activePreview->close();
    }
    foreach(PageDesignIntf* page,m_pages) delete page;
    m_pages.clear();

    foreach(ReportTranslation* translation, m_translations)
        delete translation;
    m_translations.clear();

    if (m_ownedSettings&&m_settings) delete m_settings;
}

QObject* LimeRenderPrivate::createElement(const QString &, const QString &)
{
    return appendPage();
}

QObject *LimeRenderPrivate::elementAt(const QString &, int index)
{
    return pageAt(index);
}

PageDesignIntf *LimeRenderPrivate::createPage(const QString &pageName)
{
    PageDesignIntf* page =new PageDesignIntf();
    page->setObjectName(pageName);
    page->pageItem()->setObjectName("Report"+pageName);
    page->setReportRender(this);
    page->setReportSettings(&m_reportSettings);
    return page;
}

PageDesignIntf *LimeRenderPrivate::appendPage(const QString &pageName)
{
    PageDesignIntf* page = createPage(pageName);
    m_pages.append(page);
    return page;
}

bool LimeRenderPrivate::deletePage(PageDesignIntf *page){
    QList<PageDesignIntf*>::iterator it = m_pages.begin();
    while (it != m_pages.end()){
        if (*it == page) {
            it = m_pages.erase(it);
            return true;
        } else ++it;
    }
    return false;
}

PageDesignIntf *LimeRenderPrivate::createPreviewPage()
{
    return createPage();
}

int LimeRenderPrivate::elementsCount(const QString &)
{
    return m_pages.count();
}

void LimeRenderPrivate::collectionLoadFinished(const QString &)
{
    foreach (PageDesignIntf* page, m_pages) {
        page->setReportRender(this);
        page->setReportSettings(&m_reportSettings);
        page->setSceneRect(-Const::SCENE_MARGIN,-Const::SCENE_MARGIN,
                           page->pageItem()->width()+Const::SCENE_MARGIN*2,
                           page->pageItem()->height()+Const::SCENE_MARGIN*2);
    }
    emit pagesLoadFinished();
}

void LimeRenderPrivate::saveError(QString message)
{
    m_lastError = message;
}

void LimeRenderPrivate::showError(QString message)
{
    QMessageBox::critical(0,tr("Error"),message);
}

void LimeRenderPrivate::updateTranslations()
{
    foreach(ReportTranslation* translation, m_translations.values()){
        foreach(PageDesignIntf* page, m_pages){
            translation->updatePageTranslation(page);
        }
    }
}

void LimeRenderPrivate::slotDataSourceCollectionLoaded(const QString &collectionName)
{
    emit datasourceCollectionLoadFinished(collectionName);
}

void LimeRenderPrivate::slotPreviewWindowDestroyed(QObject* window)
{
    if (m_activePreview == window){
        m_activePreview = 0;
    }
}

void LimeRenderPrivate::clearReport()
{
    foreach(PageDesignIntf* page,m_pages) delete page;
    m_pages.clear();
    foreach(ReportTranslation* reportTranslation, m_translations)
        delete reportTranslation;
    m_translations.clear();
    m_datasources->clear(DataSourceManager::Owned);
    m_fileName="";
    m_scriptEngineContext->clear();
    m_reportSettings.setDefaultValues();

    emit cleared();
}

void LimeRenderPrivate::printReport(ItemsReaderIntf::Ptr reader, QPrinter& printer)
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

void LimeRenderPrivate::printReport(ReportPages pages, QPrinter &printer)
{
    LimeReport::PageDesignIntf renderPage;
    renderPage.setItemMode(PrintMode);
    QPainter* painter=0;

    bool isFirst = true;
    int currenPage = 1;


    qreal leftMargin, topMargin, rightMargin, bottomMargin;
    printer.getPageMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin, QPrinter::Millimeter);

    QRectF printerPageRect = printer.pageRect(QPrinter::Millimeter);
    printerPageRect = QRectF(0,0,(printerPageRect.size().width() + rightMargin + leftMargin) * Const::mmFACTOR,
                                 (printerPageRect.size().height() + bottomMargin +topMargin) * Const::mmFACTOR);

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
                    if (page->getSetPageSizeToPrinter())
                      printer.setPaperSize(pageSize,QPrinter::Millimeter);
                } else {
                    if (page->getSetPageSizeToPrinter())
                      printer.setPaperSize((QPrinter::PageSize)renderPage.pageItem()->pageSize());
                }
            }

            if (!isFirst){
                printer.newPage();
            } else {
                isFirst=false;
                painter = new QPainter(&printer);
            }            

            if (printerPageRect.width() < page->geometry().width()){
                qreal pageWidth = page->geometry().width();
                QRectF currentPrintingRect = printerPageRect;
                while (pageWidth>0){
                    renderPage.render(painter, printer.pageRect(), currentPrintingRect);
                    currentPrintingRect.adjust(printerPageRect.size().width(),0,printerPageRect.size().width(),0);
                    pageWidth -= printerPageRect.size().width();
                    if (pageWidth>0) printer.newPage();
                }

            } else {
               renderPage.render(painter);
            }


            page->setPos(pagePos);
        }

        currenPage++;
    }
    delete painter;
}

QStringList LimeRenderPrivate::aviableReportTranslations()
{
    QStringList result;
    foreach (QLocale::Language language, aviableLanguages()){
        result << QLocale::languageToString(language);
    }
    return result;
}

void LimeRenderPrivate::setReportTranslation(const QString &languageName)
{
    foreach(QLocale::Language language, aviableLanguages()){
       if (QLocale::languageToString(language).compare(languageName) == 0){
           setReportLanguage(language);
       }
    }
};

bool LimeRenderPrivate::printReport(QPrinter* printer)
{
    if (!printer&&!m_printerSelected){
        QPrinterInfo pi;
        if (!pi.defaultPrinter().isNull())
#ifdef HAVE_QT4
            m_printer.data()->setPrinterName(pi.defaultPrinter().printerName());
#endif
#ifdef HAVE_QT5
            m_printer.data()->setPrinterName(pi.defaultPrinterName());
#endif
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

bool LimeRenderPrivate::printPages(ReportPages pages, QPrinter *printer)
{
    if (!printer&&!m_printerSelected){
        QPrinterInfo pi;
        if (!pi.defaultPrinter().isNull())
#ifdef HAVE_QT4
            m_printer.data()->setPrinterName(pi.defaultPrinter().printerName());
#endif
#ifdef HAVE_QT5
            m_printer.data()->setPrinterName(pi.defaultPrinterName());
#endif
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

void LimeRenderPrivate::printToFile(const QString &fileName)
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

bool LimeRenderPrivate::printToPDF(const QString &fileName)
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

void LimeRenderPrivate::previewReport(PreviewHints hints)
{ 
//    QTime start = QTime::currentTime();
    try{
        dataManager()->setDesignTime(false);
        ReportPages pages = renderToPages();
        dataManager()->setDesignTime(true);
        if (pages.count()>0){
            Q_Q(LimeRender);
            PreviewReportWindow* w = new PreviewReportWindow(q,0,settings());
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

PreviewReportWidget* LimeRenderPrivate::createPreviewWidget(QWidget* parent){

    Q_Q(LimeRender);
    PreviewReportWidget* widget = new PreviewReportWidget(q, parent);
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

PageDesignIntf* LimeRenderPrivate::createPreviewScene(QObject* parent){
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

bool LimeRenderPrivate::emitLoadReport()
{
    bool result = false;
    emit onLoad(result);
    return result;
}

bool LimeRenderPrivate::slotLoadFromFile(const QString &fileName)
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

void LimeRenderPrivate::cancelRender()
{
    if (m_LimeRender)
        m_LimeRender->cancelRender();
    m_LimeRendering = false;
}

PageDesignIntf* LimeRender::createPreviewScene(QObject* parent){
    Q_D(LimeRender);
    return d->createPreviewScene(parent);
}

void LimeRenderPrivate::setSettings(QSettings* value)
{
    if (value){
        if (m_ownedSettings&&m_settings)
            delete m_settings;
        m_settings = value;
        m_ownedSettings = false;
    }
}

QSettings*LimeRenderPrivate::settings()
{
    if (m_settings){
        return m_settings;
    } else {
        m_settings = new QSettings("LimeReport",QApplication::applicationName());
        m_ownedSettings=true;
        return m_settings;
    }
}

bool LimeRenderPrivate::loadFromFile(const QString &fileName, bool autoLoadPreviewOnChange)
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

bool LimeRenderPrivate::loadFromByteArray(QByteArray* data, const QString &name){
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

bool LimeRenderPrivate::loadFromString(const QString &report, const QString &name)
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

QString LimeRenderPrivate::renderToString()
{
    LimeReport::LimeRender render;
    dataManager()->connectAllDatabases();
    dataManager()->setDesignTime(false);
    if (m_pages.count()){
        render.setDatasources(dataManager());
        render.setScriptContext(scriptContext());
        return render.renderPageToString(m_pages.at(0));
    }else return QString();
}

PageDesignIntf* LimeRenderPrivate::getPageByName(const QString& pageName)
{
    foreach(PageDesignIntf* page, m_pages){
        if ( page->objectName().compare(pageName, Qt::CaseInsensitive) == 0)
            return page;
    }
    return 0;
}

Qt::LayoutDirection LimeRenderPrivate::previewLayoutDirection()
{
    return m_previewLayoutDirection;
}

void LimeRenderPrivate::setPreviewLayoutDirection(const Qt::LayoutDirection& layoutDirection)
{
    m_previewLayoutDirection = layoutDirection;
}

void LimeRenderPrivate::setPassPhrase(const QString &passPhrase)
{
    m_passPhrase = passPhrase;
}

void LimeRenderPrivate::reorderPages(const QList<PageDesignIntf *>& reorderedPages)
{
    m_pages.clear();
    foreach(PageDesignIntf* page, reorderedPages){
        m_pages.append(page);
    }
}

void LimeRenderPrivate::clearSelection()
{
    foreach (PageDesignIntf* page, m_pages) {
        foreach(QGraphicsItem* item, page->selectedItems()){
            item->setSelected(false);
        }
    }
}

bool LimeRenderPrivate::addTranslationLanguage(QLocale::Language language)
{
    if (!m_translations.keys().contains(language)){
        ReportTranslation* translation = 0;
        if (!m_translations.contains(QLocale::AnyLanguage)){
            translation = new ReportTranslation(QLocale::AnyLanguage,m_pages);
            m_translations.insert(QLocale::AnyLanguage,translation);
        }
        translation = new ReportTranslation(language,m_pages);
        m_translations.insert(language, translation);
        return true;
    } else {
        m_lastError =  tr("Language %1 already exists").arg(QLocale::languageToString(language));
        return false;
    }
}

bool LimeRenderPrivate::removeTranslationLanguage(QLocale::Language language)
{
    return m_translations.remove(language) != 0;
}

void LimeRenderPrivate::activateLanguage(QLocale::Language language)
{
    if (!m_translations.keys().contains(language)) return;
    ReportTranslation* translation = m_translations.value(language);

    foreach(PageTranslation* pageTranslation, translation->pagesTranslation()){
        PageDesignIntf* page = getPageByName(pageTranslation->pageName);
        if (page){
            foreach(ItemTranslation* itemTranslation, pageTranslation->itemsTranslation){
                BaseDesignIntf* item = page->pageItem()->childByName(itemTranslation->itemName);
                if (item) {
                    foreach(PropertyTranslation* propertyTranslation, itemTranslation->propertyesTranslation){
                        item->setProperty(propertyTranslation->propertyName.toLatin1(), propertyTranslation->value);
                    }
                }
            }
        }
    }
}

bool LimeRenderPrivate::setReportLanguage(QLocale::Language language){
    m_reportLanguage = language;
    if (!m_translations.keys().contains(language)) return false;
//    activateLanguage(language);
    return true;
}

QList<QLocale::Language> LimeRenderPrivate::aviableLanguages()
{
    return  m_translations.keys();
}

ReportTranslation*LimeRenderPrivate::reportTranslation(QLocale::Language language)
{
    return m_translations.value(language);
}

bool LimeRenderPrivate::resultIsEditable() const
{
    return m_resultIsEditable;
}

void LimeRenderPrivate::setResultEditable(bool value)
{
    m_resultIsEditable = value;
}

bool LimeRenderPrivate::suppressFieldAndVarError() const
{
    return m_reportSettings.suppressAbsentFieldsAndVarsWarnings();
}

void LimeRenderPrivate::setSuppressFieldAndVarError(bool suppressFieldAndVarError)
{
    m_reportSettings.setSuppressAbsentFieldsAndVarsWarnings(suppressFieldAndVarError);
}

bool LimeRenderPrivate::isBusy()
{
    return m_LimeRendering;
}

QString LimeRenderPrivate::previewWindowTitle() const
{
    return m_previewWindowTitle;
}

void LimeRenderPrivate::setPreviewWindowTitle(const QString &previewWindowTitle)
{
    m_previewWindowTitle = previewWindowTitle;
}

QIcon LimeRenderPrivate::previewWindowIcon() const
{
    return m_previewWindowIcon;
}

void LimeRenderPrivate::setPreviewWindowIcon(const QIcon &previewWindowIcon)
{
    m_previewWindowIcon = previewWindowIcon;
}

ReportPages LimeRenderPrivate::renderToPages()
{
    if (m_LimeRendering) return ReportPages();
    m_LimeRender = LimeRender::Ptr(new LimeRender);

    dataManager()->clearErrors();
    dataManager()->connectAllDatabases();
    dataManager()->setDesignTime(false);
    dataManager()->updateDatasourceModel();

    connect(m_LimeRender.data(),SIGNAL(pageRendered(int)),
            this, SIGNAL(renderPageFinished(int)));

    if (m_pages.count()){
#ifdef HAVE_UI_LOADER
        m_scriptEngineContext->initDialogs();
#endif
        ReportPages result;
        m_LimeRendering = true;

        m_LimeRender->setDatasources(dataManager());
        m_LimeRender->setScriptContext(scriptContext());

        foreach (PageDesignIntf* page, m_pages) {
            scriptContext()->baseDesignIntfToScript(page->pageItem()->objectName(), page->pageItem());
        }

        scriptContext()->qobjectToScript("engine",this);

        if (m_scriptEngineContext->runInitScript()){

            activateLanguage(m_reportLanguage);
            emit renderStarted();

            foreach(PageDesignIntf* page , m_pages){
                if (!page->pageItem()->isTOC()){
                    page->setReportSettings(&m_reportSettings);
                    result.append(m_LimeRender->renderPageToPages(page));
                }
            }


//            m_LimeRender->secondRenderPass(result);

            for (int i=0; i<m_pages.count(); ++i){
                 PageDesignIntf* page = m_pages.at(i);
                if (page->pageItem()->isTOC()){
                    page->setReportSettings(&m_reportSettings);
                    if (i==0){
                        PageDesignIntf* secondPage = 0;
                        if (m_pages.count()>1) secondPage = m_pages.at(1);
                        ReportPages pages = m_LimeRender->renderTOC(
                                    page,
                                    true,
                                    secondPage && secondPage->pageItem()->resetPageNumber()
                        );
                        for (int j=0; j<pages.count(); ++j){
                            result.insert(j,pages.at(j));
                        }

                    } else {
                        result.append(m_LimeRender->renderPageToPages(page));
                    }
                }
            }

            m_LimeRender->secondRenderPass(result);

            emit renderFinished();
            m_LimeRender.clear();
        }
        m_LimeRendering = false;
        activateLanguage(QLocale::AnyLanguage);
        return result;
    } else {
        return ReportPages();
    }
}

QString LimeRenderPrivate::lastError()
{
    return m_lastError;
}

LimeRender::LimeRender(QObject *parent)
    : QObject(parent), d_ptr(new LimeRenderPrivate())
{
    Q_D(LimeRender);
    d->q_ptr=this;
    connect(d, SIGNAL(renderStarted()), this, SIGNAL(renderStarted()));
    connect(d, SIGNAL(renderPageFinished(int)),
            this, SIGNAL(renderPageFinished(int)));
    connect(d, SIGNAL(renderFinished()), this, SIGNAL(renderFinished()));
    connect(d, SIGNAL(onLoad(bool&)), this, SIGNAL(onLoad(bool&)));
}

LimeRender::~LimeRender()
{
    delete d_ptr;
}

bool LimeRender::printReport(QPrinter *printer)
{
    Q_D(LimeRender);
    return d->printReport(printer);
}

bool LimeRender::printPages(ReportPages pages, QPrinter *printer){
    Q_D(LimeRender);
    return d->printPages(pages,printer);
}

void LimeRender::printToFile(const QString &fileName)
{
    Q_D(LimeRender);
    d->printToFile(fileName);
}

bool LimeRender::printToPDF(const QString &fileName)
{
    Q_D(LimeRender);
    return d->printToPDF(fileName);
}

void LimeRender::previewReport(PreviewHints hints)
{
    Q_D(LimeRender);
    if (m_settings)
        d->setSettings(m_settings);
    d->previewReport(hints);
}

void LimeRender::designReport()
{
    Q_D(LimeRender);
    if (m_settings)
        d->setSettings(m_settings);
    d->designReport();
}

PreviewReportWidget* LimeRender::createPreviewWidget(QWidget *parent)
{
    Q_D(LimeRender);
    return d->createPreviewWidget(parent);
}

void LimeRender::setPreviewWindowTitle(const QString &title)
{
    Q_D(LimeRender);
    d->setPreviewWindowTitle(title);
}

void LimeRender::setPreviewWindowIcon(const QIcon &icon)
{
    Q_D(LimeRender);
    d->setPreviewWindowIcon(icon);
}

void LimeRender::setResultEditable(bool value)
{
    Q_D(LimeRender);
    d->setResultEditable(value);
}

bool LimeRender::resultIsEditable()
{
    Q_D(LimeRender);
    return d->resultIsEditable();
}

bool LimeRender::isBusy()
{
    Q_D(LimeRender);
    return d->isBusy();
}

void LimeRender::setPassPharse(QString &passPharse)
{
    Q_D(LimeRender);
    d->setPassPhrase(passPharse);
}

QList<QLocale::Language> LimeRender::aviableLanguages()
{
    Q_D(LimeRender);
    return d->aviableLanguages();
}

bool LimeRender::setReportLanguage(QLocale::Language language)
{
    Q_D(LimeRender);
    return d->setReportLanguage(language);
}

void LimeRender::setShowProgressDialog(bool value)
{
    Q_D(LimeRender);
    d->setShowProgressDialog(value);
}

IDataSourceManager *LimeRender::dataManager()
{
    Q_D(LimeRender);
    return d->dataManagerIntf();
}

IScriptEngineManager *LimeRender::scriptManager()
{
    Q_D(LimeRender);
    return d->scriptManagerIntf();
}

bool LimeRender::loadFromFile(const QString &fileName, bool autoLoadPreviewOnChange)
{
    Q_D(LimeRender);
    return d->loadFromFile(fileName, autoLoadPreviewOnChange);
}

bool LimeRender::loadFromByteArray(QByteArray* data){
    Q_D(LimeRender);
    return d->loadFromByteArray(data);
}

bool LimeRender::loadFromString(const QString &data)
{
    Q_D(LimeRender);
    return d->loadFromString(data);
}

QString LimeRender::reportFileName()
{
    Q_D(LimeRender);
    return d->reportFileName();
}

void LimeRender::setReportFileName(const QString &fileName)
{
    Q_D(LimeRender);
    return d->setReportFileName(fileName);
}

QString LimeRender::lastError()
{
    Q_D(LimeRender);
    return d->lastError();
}

void LimeRender::setCurrentReportsDir(const QString &dirName)
{
    Q_D(LimeRender);
    return d->setCurrentReportsDir(dirName);
}

void LimeRender::setReportName(const QString &name)
{
    Q_D(LimeRender);
    return d->setReportName(name);
}

QString LimeRender::reportName()
{
    Q_D(LimeRender);
    return d->reportName();
}

void LimeRender::cancelRender()
{
    Q_D(LimeRender);
    d->cancelRender();
}

LimeRender::LimeRender(LimeRenderPrivate &dd, QObject *parent)
    :QObject(parent),d_ptr(&dd)
{
    Q_D(LimeRender);
    d->q_ptr=this;
    connect(d, SIGNAL(renderStarted()), this, SIGNAL(renderStarted()));
    connect(d, SIGNAL(renderPageFinished(int)),
            this, SIGNAL(renderPageFinished(int)));
    connect(d, SIGNAL(renderFinished()), this, SIGNAL(renderFinished()));
}

ScriptEngineManager*LimeReport::LimeRenderPrivate::scriptManager(){
    ScriptEngineManager::instance().setContext(scriptContext());
    ScriptEngineManager::instance().setDataManager(dataManager());
    return &ScriptEngineManager::instance();
}

}// namespace LimeReport

