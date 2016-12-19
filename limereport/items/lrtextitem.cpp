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
#include <QtGui>
#include <QTextLayout>
#include <QtScript/QScriptEngine>
#include <QLocale>
#include <math.h>

#include "lrpagedesignintf.h"
#include "lrtextitem.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"
#include "lrsimpletagparser.h"
#include "lrtextitemeditor.h"
#include "lrreportengine_p.h"

namespace{

const QString xmlTag = "TextItem";

LimeReport::BaseDesignIntf * createTextItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::TextItem(owner,parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(xmlTag, LimeReport::ItemAttribs(QObject::tr("Text Item"),"TextItem"), createTextItem);

}

namespace LimeReport{

TextItem::TextItem(QObject *owner, QGraphicsItem *parent)
    : ContentItemDesignIntf(xmlTag,owner,parent), m_angle(Angle0), m_trimValue(true), m_allowHTML(false),
      m_allowHTMLInFields(false)
{
    PageItemDesignIntf* pageItem = dynamic_cast<PageItemDesignIntf*>(parent);
    BaseDesignIntf* parentItem = dynamic_cast<BaseDesignIntf*>(parent);
    while (!pageItem && parentItem){
        parentItem = dynamic_cast<BaseDesignIntf*>(parentItem->parentItem());
        pageItem = dynamic_cast<PageItemDesignIntf*>(parentItem);
    }

    if (pageItem){
        QFont defaultFont = pageItem->font();
        setFont(defaultFont);
    }
    Init();
}

TextItem::~TextItem()
{

}

int TextItem::fakeMarginSize(){
    return marginSize()+5;
}

void TextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
    Q_UNUSED(widget);
    Q_UNUSED(style);

    painter->save();

    setupPainter(painter);
    prepareRect(painter,style,widget);

    QSizeF tmpSize = rect().size()-m_textSize;

    TextPtr text = textDocument();

    if (!painter->clipRegion().isEmpty()){
        QRegion clipReg=painter->clipRegion().xored(painter->clipRegion().subtracted(rect().toRect()));
        painter->setClipRegion(clipReg);
    } else {
        painter->setClipRect(rect());
    }

    qreal hOffset = 0, vOffset=0;
    switch (m_angle){
        case Angle0:
            hOffset = fakeMarginSize();
            if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignVCenter)){
                vOffset = tmpSize.height() / 2;
            }
            if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignBottom)) // allow html
                vOffset = tmpSize.height();
            painter->translate(hOffset,vOffset);
        break;
        case Angle90:
            hOffset = width() - fakeMarginSize();
            vOffset = fakeMarginSize();
            if (m_alignment & Qt::AlignVCenter){
                hOffset = (width() - text->size().height()) / 2 + text->size().height();
            }

            if (m_alignment & Qt::AlignBottom){
                hOffset = (text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(90);
        break;
        case Angle180:
            hOffset = width()-fakeMarginSize();
            vOffset = height()-fakeMarginSize();
            if ((tmpSize.width() > 0) && (m_alignment & Qt::AlignVCenter)){
                vOffset = tmpSize.height() / 2 + text->size().height();
            }
            if ((tmpSize.height() > 0) && (m_alignment & Qt::AlignBottom)){
                vOffset = (text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(180);
        break;
        case Angle270:
            hOffset = fakeMarginSize();
            vOffset = height() - fakeMarginSize();
            if (m_alignment & Qt::AlignVCenter){
                hOffset = (width() - text->size().height()) / 2;
            }

            if (m_alignment & Qt::AlignBottom){
                hOffset = (width() - text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(270);
        break;
        case Angle45:
            painter->translate(width() / 2,0);
            painter->rotate(45);
            text->setTextWidth(sqrt( 2 * (pow(width() / 2, 2))));
        break;
        case Angle315:
            painter->translate(0,height() / 2);
            painter->rotate(315);
            text->setTextWidth(sqrt( 2 * (pow(height() / 2, 2))));
        break;
    }

    painter->setOpacity(qreal(foregroundOpacity())/100);

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, fontColor());
    text->documentLayout()->draw(painter,ctx);

    painter->restore();
    BaseDesignIntf::paint(painter, style, widget);
}

QString TextItem::content() const{
    return m_strText;
}

void TextItem::Init()
{
    m_autoWidth = NoneAutoWidth;
    m_alignment = Qt::AlignLeft|Qt::AlignTop;
    m_autoHeight = false;
    m_textSize = QSizeF();
    m_firstLineSize = 0;
    m_foregroundOpacity = 100;
    m_valueType = Default;
}

void TextItem::setContent(const QString &value)
{
    if (m_strText.compare(value)!=0){
        QString oldValue = m_strText;
        m_strText=value;

        if (itemMode() == DesignMode){
            initTextSizes();
        }

        if (!isLoading()){
            update(rect());
            notify("content",oldValue,value);
        }
    }
}

void TextItem::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{
    if (isNeedExpandContent())
        expandContent(dataManager, pass);
    if (!isLoading())
        initTextSizes();

    if (m_textSize.width()>width() && ((m_autoWidth==MaxWordLength)||(m_autoWidth==MaxStringLength))){
        setWidth(m_textSize.width() + fakeMarginSize()*2);
    }

    if ((m_textSize.height()>height()) && (m_autoHeight) ){
        setHeight(m_textSize.height()+5);
    }
    BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
}

void TextItem::updateLayout()
{
//    m_layout.setFont(transformToSceneFont(font()));
//    m_layout.setText(content());
//    qreal linePos = 0;
//    m_layout.beginLayout();
//    while(true){
//        QTextLine line = m_layout.createLine();
//        if (!line.isValid()) break;
//        line.setLineWidth(width()-marginSize()*2);
//        line.setPosition(QPoint(marginSize(),linePos));
//        linePos+=line.height();
//    }
//    m_layout.endLayout();
}

bool TextItem::isNeedExpandContent() const
{
    QRegExp rx("$*\\{[^{]*\\}");
    return content().contains(rx);
}

QString TextItem::replaceBR(QString text)
{
    return text.replace("<br/>","\n");
}

QString TextItem::replaceReturns(QString text)
{
    QString result = text.replace("\r\n","<br/>");
    result = result.replace("\n","<br/>");
    return result;
}

void TextItem::initTextSizes()
{
    TextPtr text = textDocument();
    m_textSize= text->size();
    if (text->begin().isValid() && text->begin().layout()->lineAt(0).isValid())
        m_firstLineSize = text->begin().layout()->lineAt(0).height();
}

QString TextItem::formatDateTime(const QDateTime &value)
{
    if (m_format.isEmpty())
    {
        return value.toString();
    }

    return value.toString(m_format);
}

QString TextItem::formatNumber(const double value)
{
    QString str = QString::number(value);

    if (m_format.contains("%"))
    {
        str.sprintf(m_format.toStdString().c_str(), value);
        str = str.replace(",", QLocale::system().groupSeparator());
        str = str.replace(".", QLocale::system().decimalPoint());
    }

    return str;
}

QString TextItem::formatFieldValue()
{
    if (m_format.isEmpty()) {
        return m_varValue.toString();
    }

    QVariant value = m_varValue;

    if (m_valueType != Default) {
        switch (m_valueType) {
        case DateTime:
            {
                QDateTime dt = QDateTime::fromString(value.toString(), Qt::ISODate);
                value = (dt.isValid() ? QVariant(dt) : m_varValue);
                break;
            }
        case Double:
            {
                bool bOk = false;
                double dbl = value.toDouble(&bOk);
                value = (bOk ? QVariant(dbl) : m_varValue);
            }
        default: break;
        }
    }

    switch (value.type()) {
    case QVariant::Date:
    case QVariant::DateTime:
        return formatDateTime(value.toDateTime());
    case QVariant::Double:
        return formatNumber(value.toDouble());
    default:
        return value.toString();
    }
}

TextItem::TextPtr TextItem::textDocument()
{
    TextPtr text(new QTextDocument);
    QTextOption to;
    to.setAlignment(m_alignment);

    if (m_autoWidth!=MaxStringLength)
        to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else to.setWrapMode(QTextOption::NoWrap);

    text->setDocumentMargin(0);
    text->setDefaultTextOption(to);
    text->setDefaultFont(transformToSceneFont(font()));

    if (allowHTML())
        text->setHtml(replaceReturns(m_strText.trimmed()));
    else
        text->setPlainText(m_strText);

    if ((m_angle==Angle0)||(m_angle==Angle180)){
        text->setTextWidth(rect().width()-fakeMarginSize()*2);
    } else {
        text->setTextWidth(rect().height()-fakeMarginSize()*2);
    }

    m_textSize = text->size();

    return text;

}

TextItem::ValueType TextItem::valueType() const
{
    return m_valueType;
}

void TextItem::setValueType(const ValueType valueType)
{
    m_valueType = valueType;
}

QString TextItem::format() const
{
    return m_format;
}

void TextItem::setFormat(const QString &format)
{
    m_format = format;
}

bool TextItem::allowHTMLInFields() const
{
    return m_allowHTMLInFields;
}

void TextItem::setAllowHTMLInFields(bool allowHTMLInFields)
{
    if (m_allowHTMLInFields != allowHTMLInFields){
        m_allowHTMLInFields = allowHTMLInFields;
        notify("allowHTMLInFields",!m_allowHTMLInFields,allowHTMLInFields);
        update();
    }
}

bool TextItem::allowHTML() const
{
    return m_allowHTML;
}

void TextItem::setAllowHTML(bool allowHTML)
{
    if (m_allowHTML!=allowHTML){
        m_allowHTML = allowHTML;
        update();
        notify("allowHTML",!m_allowHTML,allowHTML);
    }
}
bool TextItem::trimValue() const
{
    return m_trimValue;
}

void TextItem::setTrimValue(bool trimValue)
{
    m_trimValue = trimValue;
}


void TextItem::geometryChangedEvent(QRectF , QRectF)
{
//    if ((m_angle==Angle0)||(m_angle==Angle180)){
//        m_text->setTextWidth(rect().width()-fakeMarginSize()*2);
//    } else {
//        m_text->setTextWidth(rect().height()-fakeMarginSize()*2);
//    }
//    m_textSize=m_text->size();
    if (itemMode() == DesignMode) initTextSizes();
}

bool TextItem::isNeedUpdateSize(RenderPass pass) const
{
    Q_UNUSED(pass)
    bool res =  (m_textSize.height()>geometry().height()&&autoHeight()) ||
                (m_textSize.width()>geometry().width()&&autoWidth()) ||
                isNeedExpandContent();
    return res;
}

void TextItem::setAlignment(Qt::Alignment value)
{
    if (m_alignment!=value){
        Qt::Alignment oldValue = m_alignment;
        m_alignment=value;
        //m_layout.setTextOption(QTextOption(m_alignment));
        if (!isLoading()){
            initTextSizes();
            update(rect());
            notify("alignment",QVariant(oldValue),QVariant(value));
        }
    }
}

void TextItem::expandContent(DataSourceManager* dataManager, RenderPass pass)
{
    QString context=content();
    ExpandType expandType = (allowHTML() && !allowHTMLInFields())?ReplaceHTMLSymbols:NoEscapeSymbols;
    switch(pass){
    case FirstPass:
        context=expandUserVariables(context, pass, expandType, dataManager);
        context=expandScripts(context, dataManager);
        context=expandDataFields(context, expandType, dataManager);
        break;
    case SecondPass:;
        context=expandUserVariables(context, pass, expandType, dataManager);
        context=expandScripts(context, dataManager);
    }

    if (expandType == NoEscapeSymbols && !m_varValue.isNull() &&m_valueType!=Default) {
        setContent(formatFieldValue());
    } else {
        setContent(context);
    }
}

void TextItem::setAutoHeight(bool value)
{
    if (m_autoHeight!=value){
        bool oldValue = m_autoHeight;
        m_autoHeight=value;
        notify("autoHeight",oldValue,value);
    }
}

void TextItem::setAutoWidth(TextItem::AutoWidth value)
{
    if (m_autoWidth!=value){
        TextItem::AutoWidth oldValue = m_autoWidth;
        m_autoWidth=value;
        notify("autoWidth",oldValue,value);
    }
}

bool TextItem::canBeSplitted(int height) const
{
    return height>m_firstLineSize;
}

BaseDesignIntf *TextItem::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextPtr text = textDocument();
    int linesHeight=0;
    QString tmpText="";
    TextItem* upperPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));

    for (QTextBlock it=text->begin();it!=text->end();it=it.next()){
        for (int i=0;i<it.layout()->lineCount();i++){
          linesHeight+=it.layout()->lineAt(i).height();
          if (linesHeight>(height-(fakeMarginSize()*2+borderLineSize()*2))) {
              linesHeight-=it.layout()->lineAt(i).height();
              goto loop_exit;
          }
          tmpText+=it.text().mid(it.layout()->lineAt(i).textStart(),it.layout()->lineAt(i).textLength())+'\n';
        }
    }
    loop_exit:
    tmpText.chop(1);

    upperPart->setHeight(linesHeight+fakeMarginSize()*2+borderLineSize()*2);
    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    upperPart->setContent(context->extendTextByTags(tmpText,0));
    upperPart->initTextSizes();
    return upperPart;
}

BaseDesignIntf *TextItem::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextPtr text = textDocument();
    TextItem* bottomPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    int linesHeight=0;
    int curLine=0;
    QTextBlock curBlock;

    QString tmpText="";

    for (curBlock=text->begin();curBlock!=text->end();curBlock=curBlock.next()){
        for (curLine=0;curLine<curBlock.layout()->lineCount();curLine++){
            linesHeight+=curBlock.layout()->lineAt(curLine).height();
            if (linesHeight>(height-(fakeMarginSize()*2+borderLineSize()*2))) {goto loop_exit;}
        }
    }
    loop_exit:;

    int textPos=0;
    for (;curBlock!=text->end();curBlock=curBlock.next(),curLine=0){
        for (;curLine<curBlock.layout()->lineCount();curLine++){
            if (tmpText=="") textPos= curBlock.layout()->lineAt(curLine).textStart();
            tmpText+=curBlock.text().mid(curBlock.layout()->lineAt(curLine).textStart(),
              curBlock.layout()->lineAt(curLine).textLength()) + "\n";
        }
    }
    tmpText.chop(1);

    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    bottomPart->setContent(context->extendTextByTags(tmpText,textPos));
    bottomPart->initTextSizes();
    bottomPart->setHeight(bottomPart->m_textSize.height()+borderLineSize()*2);
    return bottomPart;
}

BaseDesignIntf *TextItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new TextItem(owner,parent);
}

BaseDesignIntf *TextItem::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem* empty=dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    empty->setContent("");
    empty->setHeight(height);
    return empty;
}

void TextItem::objectLoadFinished()
{
    ItemDesignIntf::objectLoadFinished();
    if (itemMode() == DesignMode || !isNeedExpandContent()){
        initTextSizes();
    }
}

void TextItem::setTextItemFont(QFont value)
{
    if (font()!=value){
        QFont oldValue = font();
        setFont(value);
        update();
        notify("font",oldValue,value);
    }
}

QWidget *TextItem::defaultEditor()
{
    QSettings* l_settings = (page()->settings() != 0) ?
                                 page()->settings() :
                                 (page()->reportEditor()!=0) ? page()->reportEditor()->settings() : 0;
    QWidget* editor = new TextItemEditor(this,page(),l_settings);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    return editor;
}

void TextItem::setBackgroundOpacity(int value)
{
    if (opacity()!=value){
        int oldValue = opacity();
        setOpacity(value);
        notify("backgroundOpacity",oldValue,value);
    }
}

void TextItem::setBackgroundModeProperty(BaseDesignIntf::BGMode value)
{
    if (value!=backgroundMode()){
        BaseDesignIntf::BGMode oldValue = backgroundMode();
        setBackgroundMode(value);
        notify("backgroundMode",oldValue,value);
    }
}

void TextItem::setBackgroundColorProperty(QColor value)
{
    if(value!=backgroundColor()){
        QColor oldValue = backgroundColor();
        setBackgroundColor(value);
        notify("backgroundColor",oldValue,value);
    }
}

void TextItem::setFontColorProperty(QColor value)
{
    if(value!=fontColor()){
        QColor oldValue = fontColor();
        setFontColor(value);
        notify("fontColor",oldValue,value);
    }
}

TextItem::AngleType TextItem::angle() const
{
    return m_angle;
}

void TextItem::setAngle(const AngleType& value)
{
    if (m_angle!=value){
        AngleType oldValue = m_angle;
        m_angle = value;
        if (!isLoading()){
            initTextSizes();
            update();
            notify("angle",oldValue,value);
        }
    }
}

void TextItem::setForegroundOpacity(int value)
{
    if (value>100){
        value=100;
    } else if(value<0){
        value=0;
    }
    if (m_foregroundOpacity != value){
        int oldValue = m_foregroundOpacity;
        m_foregroundOpacity = value;
        update();
        notify("foregroundOpacity",oldValue,value);
    }
}

} //namespace LimeReport



