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
#ifndef LRREPORTDESIGNINTF_H
#define LRREPORTDESIGNINTF_H

#include <QObject>
#include <QSettings>
#include <QPrintDialog>
//#include <QJSEngine>

#include "lrglobal.h"
#include "lrdatasourcemanagerintf.h"
#include "lrscriptenginemanagerintf.h"
#include "lrpreviewreportwidget.h"
#include "lrreportdesignwindowintrerface.h"
#include "lrpreparedpagesintf.h"

class QPrinter;
class QGraphicsScene;

namespace LimeReport {

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

class LIMEREPORT_EXPORT ItemGeometry{
public:
    enum Type{Millimeters, Pixels};
    ItemGeometry(qreal x, qreal y, qreal width, qreal height, Qt::Alignment anchor, Type type = Millimeters)
        :m_x(x), m_y(y), m_width(width), m_height(height), m_type(type), m_anchor(anchor){}
    ItemGeometry(): m_x(0), m_y(0), m_width(0), m_height(0), m_type(Millimeters){}

    qreal x() const;
    void setX(const qreal &x);

    qreal y() const;
    void setY(const qreal &y);

    qreal width() const;
    void setWidth(const qreal &width);

    qreal height() const;
    void setHeight(const qreal &height);

    Type type() const;
    void setType(const Type &type);

    Qt::Alignment anchor() const;
    void setAnchor(const Qt::Alignment &anchor);

private:
    qreal m_x;
    qreal m_y;
    qreal m_width;
    qreal m_height;
    Type m_type;
    Qt::Alignment m_anchor;
};

class LIMEREPORT_EXPORT WatermarkSetting{
public:
    WatermarkSetting(const QString& text, const ItemGeometry& geometry, const QFont& font)
        : m_text(text), m_font(font), m_opacity(50), m_geometry(geometry), m_color(QColor(Qt::black)){}
    WatermarkSetting(): m_font(QFont()), m_opacity(50), m_geometry(ItemGeometry()){}
    QString text() const;
    void setText(const QString &text);

    QFont font() const;
    void setFont(const QFont &font);

    int opacity() const;
    void setOpacity(const int &opacity);

    ItemGeometry geometry() const;
    void setGeometry(const ItemGeometry &geometry);

    QColor color() const;
    void setColor(const QColor &color);

private:
    QString m_text;
    QFont   m_font;
    int   m_opacity;
    ItemGeometry m_geometry;
    QColor m_color;
};

class ItemBuilder{
    virtual void setProperty(QString name, QVariant value) = 0;
    virtual QVariant property(QString name) = 0;
    virtual void setGeometry(ItemGeometry geometry) = 0;
    virtual ItemGeometry geometry() = 0; 
};


class DataSourceManager;
class ReportEnginePrivate;
class PageDesignIntf;
class PageItemDesignIntf;
class ReportDesignWidget;
class PreviewReportWidget;
class PreparedPages;

typedef QList< QSharedPointer<PageItemDesignIntf> > ReportPages;

class LIMEREPORT_EXPORT ReportEngine : public QObject{
    Q_OBJECT
    friend class ReportDesignWidget;
    friend class PreviewReportWidget;
    friend class TranslationEditor;
public:
    static void setSettings(QSettings *value){m_settings=value;}
public:
    explicit ReportEngine(QObject *parent = 0);
    ~ReportEngine();
    bool    printReport(QPrinter *printer=0);
    bool    printReport(QMap<QString, QPrinter*> printers, bool printToAllPrinters = false);
    bool    printPages(ReportPages pages, QPrinter *printer);
    void    printToFile(const QString& fileName);
    QGraphicsScene* createPreviewScene(QObject *parent = 0);
    bool    printToPDF(const QString& fileName);
    bool    exportReport(QString exporterName, const QString &fileName = "", const QMap<QString, QVariant>& params = QMap<QString, QVariant>());
    void    previewReport(PreviewHints hints = PreviewBarsUserSetting);
    void    previewReport(QPrinter* printer, PreviewHints hints = PreviewBarsUserSetting);
    void    designReport();
    ReportDesignWindowInterface* getDesignerWindow();
    void    setShowProgressDialog(bool value);
    bool    isShowProgressDialog();
    IDataSourceManager* dataManager();
    IScriptEngineManager* scriptManager();
    bool    loadFromFile(const QString& fileName, bool autoLoadPreviewOnChange = false);
    bool    loadFromByteArray(QByteArray *data);
    bool    loadFromString(const QString& data);
    QString reportFileName();
    void    setReportFileName(const QString& fileName);
    bool    saveToFile(const QString& fileName);
    QByteArray  saveToByteArray();
    QString saveToString();
    QString lastError();
    void setCurrentReportsDir(const QString& dirName);
    void setReportName(const QString& name);
    QString reportName();
    PreviewReportWidget *createPreviewWidget(QWidget *parent = 0);
    void setPreviewWindowTitle(const QString& title);
    void setPreviewWindowIcon(const QIcon& icon);
    void setPreviewPageBackgroundColor(QColor color);
    void setResultEditable(bool value);
    bool resultIsEditable();
    void setSaveToFileVisible(bool value);
    bool saveToFileIsVisible();
    void setPrintToPdfVisible(bool value);
    bool printToPdfIsVisible();
    void setPrintVisible(bool value);
    bool printIsVisible();
    bool isBusy();
    void setPassPhrase(QString& passPhrase);
    QList<QLocale::Language> availableLanguages();
    bool setReportLanguage(QLocale::Language language);
    Qt::LayoutDirection previewLayoutDirection();
    void setPreviewLayoutDirection(const Qt::LayoutDirection& previewLayoutDirection);
    QList<QLocale::Language> designerLanguages();
    QLocale::Language currentDesignerLanguage();
    ScaleType previewScaleType();
    int  previewScalePercent();
    void setPreviewScaleType(const ScaleType &previewScaleType, int percent = 0);
    void addWatermark(const WatermarkSetting& watermarkSetting);
    void clearWatermarks();
    IPreparedPages* preparedPages();
    bool showPreparedPages(PreviewHints hints = PreviewBarsUserSetting);
    bool prepareReportPages();
    bool printPreparedPages();
    bool showDesignerModal() const;
    void setShowDesignerModal(bool showDesignerModal);

signals:
    void cleared();
    void renderStarted();
    void renderFinished();
    void renderPageFinished(int renderedPageCount);

    void printingStarted(int pageCount);
    void printingFinished();
    void pagePrintingFinished(int index);

    void onSave(bool& saved);
    void onSaveAs(bool& saved);
    void onLoad(bool& loaded);
    void onSavePreview(bool& saved, LimeReport::IPreparedPages* pages);
    void saveFinished();
    void loadFinished();
    void printedToPDF(QString fileName);

    void getAvailableDesignerLanguages(QList<QLocale::Language>* languages);
    void currentDefaultDesignerLanguageChanged(QLocale::Language);
    QLocale::Language getCurrentDefaultDesignerLanguage();

    void  externalPaint(const QString& objectName, QPainter* painter, const QStyleOptionGraphicsItem*);

public slots:
    void cancelRender();
    void cancelPrinting();
protected:
    ReportEnginePrivate * const d_ptr;
    ReportEngine(ReportEnginePrivate &dd, QObject * parent=0);
private:
    Q_DECLARE_PRIVATE(ReportEngine)
    static QSettings* m_settings;
    bool m_showDesignerModal;
};

} // namespace LimeReport

#endif // LRREPORTDESIGNINTF_H
