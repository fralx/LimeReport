#ifndef LRRENDERENGINE_H
#define LRRENDERENGINE_H

#include <QObject>
#include <QSettings>
#include <QPrintDialog>

#include "lrglobal.h"
#include "lrdatasourcemanagerintf.h"
#include "lrscriptenginemanagerintf.h"
#include "lrpreviewreportwidget.h"

namespace LimeReport{

class PrintRange{
public:
    int fromPage() const { return m_fromPage;}
    int toPage() const { return m_toPage;}
    QPrintDialog::PrintRange rangeType() const { return m_rangeType;}
    PrintRange(QAbstractPrintDialog::PrintRange rangeType=QPrintDialog::AllPages, int fromPage=0, int toPage=0);
    void setRangeType(QAbstractPrintDialog::PrintRange rangeType){ m_rangeType=rangeType;}
    void setFromPage(int fromPage){ m_fromPage = fromPage;}
    void setToPage(int toPage){ m_toPage = toPage;}
private:
    QPrintDialog::PrintRange m_rangeType;
    int m_fromPage;
    int m_toPage;
};

class DataSourceManager;
class PageDesignIntf;
class PageItemDesignIntf;
class PreviewReportWidget;

typedef QList< QSharedPointer<PageItemDesignIntf> > ReportPages;

class RenderEnginePrivate;

class LIMEREPORT_EXPORT RenderEngine: public QObject{
    Q_OBJECT
    friend class PreviewReportWidget;
public:
    static void setSettings(QSettings *value){m_settings=value;}
public:
    explicit RenderEngine(QObject *parent = 0);
    explicit RenderEngine(RenderEnginePrivate* dd, QObject *parent = 0);
    ~RenderEngine();
    bool    printReport(QPrinter *printer=0);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    PageDesignIntf *createPreviewScene(QObject *parent = 0);
    bool    printToPDF(const QString& fileName);
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);
    IDataSourceManager* dataManager();
    IScriptEngineManager* scriptManager();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange = false);
    bool    loadFromByteArray(QByteArray *data);
    bool    loadFromString(const QString& data);
    QString reportFileName();
    void    setReportFileName(const QString& fileName);
    QString lastError();
    PreviewReportWidget *createPreviewWidget(QWidget *parent = 0);
    void setPreviewWindowTitle(const QString& title);
    void setPreviewWindowIcon(const QIcon& icon);
    void setResultEditable(bool value);
    bool resultIsEditable();
    bool isBusy();
    void setPassPharse(QString& passPharse);
    QList<QLocale::Language> aviableLanguages();
    bool setReportLanguage(QLocale::Language language);
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
    QSettings* settings(){ return m_settings;}
signals:
    void renderStarted();
    void renderFinished();
    void renderPageFinished(int renderedPageCount);
    void onLoad(bool& loaded);
public slots:
    void cancelRender();
protected:
    QObject* d_ptr;
private:
    static QSettings* m_settings;
    void init();
private:
    Q_DECLARE_PRIVATE(RenderEngine)
};

} // namespace LimeReport
#endif // LRRENDERENGINE_H
