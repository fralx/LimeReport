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
bool registred = LimeReport::DesignElementsFactory::instance().registerCreator(xmlTag, LimeReport::ItemAttribs(QObject::tr("Text Item"),"TextItem"), createTextItem);

}

namespace LimeReport{

TextItem::TextItem(QObject *owner, QGraphicsItem *parent)
    : ContentItemDesignIntf(xmlTag,owner,parent), m_angle(Angle0), m_trimValue(true), m_allowHTML(false),
      m_allowHTMLInFields(false)
{
    m_text = new QTextDocument();
    Init();
}

TextItem::~TextItem()
{
    delete m_text;
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
            if ((tmpSize.height()>0) && (m_alignment & Qt::AlignVCenter)){
                vOffset = tmpSize.height()/2;
            }
            if ((tmpSize.height()>0) && (m_alignment & Qt::AlignBottom)) // allow html
                vOffset = tmpSize.height();
            painter->translate(hOffset,vOffset);
        break;
        case Angle90:
            hOffset = width()-fakeMarginSize();
            vOffset = fakeMarginSize();
            if (m_alignment & Qt::AlignVCenter){
                hOffset = (width()-m_text->size().height())/2+m_text->size().height();
            }

            if (m_alignment & Qt::AlignBottom){
                hOffset = (m_text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(90);
        break;
        case Angle180:
            hOffset = width()-fakeMarginSize();
            vOffset = height()-fakeMarginSize();
            if ((tmpSize.width()>0) && (m_alignment & Qt::AlignVCenter)){
                vOffset = tmpSize.height()/2+m_text->size().height();
            }
            if ((tmpSize.height()>0) && (m_alignment & Qt::AlignBottom)){
                vOffset = (m_text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(180);
        break;
        case Angle270:
            hOffset = fakeMarginSize();
            vOffset = height()-fakeMarginSize();
            if (m_alignment & Qt::AlignVCenter){
                hOffset = (width()-m_text->size().height())/2;
            }

            if (m_alignment & Qt::AlignBottom){
                hOffset = (width()-m_text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(270);
        break;
        case Angle45:
            painter->translate(width()/2,0);
            painter->rotate(45);
            m_text->setTextWidth(sqrt(2*(pow(width()/2,2))));
        break;
        case Angle315:
            painter->translate(0,height()/2);
            painter->rotate(315);
            m_text->setTextWidth(sqrt(2*(pow(height()/2,2))));
        break;
    }

    int lineHeight = painter->fontMetrics().height();
    qreal curpos = 0;

    if (m_underlines){
        QPen pen = painter->pen();
        pen.setWidth(m_underlineLineSize);
        painter->setPen(pen);
    }

    painter->setOpacity(qreal(foregroundOpacity())/100);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.palette.setColor(QPalette::Text, fontColor());
    for(QTextBlock it=m_text->begin();it!=m_text->end();it=it.next()){
        it.blockFormat().setLineHeight(m_lineSpacing,QTextBlockFormat::LineDistanceHeight);
        for (int i=0;i<it.layout()->lineCount();i++){
            QTextLine line = it.layout()->lineAt(i);
            if (m_underlines){
                painter->drawLine(QPointF(0,line.rect().bottomLeft().y()),QPoint(rect().width(),line.rect().bottomRight().y()));
                lineHeight = line.height()+m_lineSpacing;
                curpos = line.rect().bottom();
            }
        }
    }
    m_text->documentLayout()->draw(painter,ctx);

    if (m_underlines){
        if (lineHeight<0) lineHeight = painter->fontMetrics().height();
        for (curpos+=lineHeight; curpos<rect().height();curpos+=lineHeight){
            painter->drawLine(QPointF(0,curpos),QPoint(rect().width(),curpos));
        }
    }

    //painter->setOpacity(qreal(foregroundOpacity())/100);

    //m_text->setDefaultTextOption();
    //QAbstractTextDocumentLayout::PaintContext ctx;
    //ctx.palette.setColor(QPalette::Text, fontColor());
    //m_text->documentLayout()->draw(painter,ctx);
    
//    m_layout.draw(ppainter,QPointF(marginSize(),0),);
//    ppainter->setFont(transformToSceneFont(font()));
//    QTextOption o;
//    o.setAlignment(alignment());
//    ppainter->drawText(rect(), content(), o);

    painter->restore();
    BaseDesignIntf::paint(painter, style, widget);
}

QString TextItem::content() const{
    return m_strText;
}

void TextItem::Init()
{
    m_autoWidth=NoneAutoWidth;
    m_alignment= Qt::AlignLeft|Qt::AlignTop;
    m_autoHeight=false;
//    m_text->setDefaultFont(transformToSceneFont(font()));
    m_textSize=QSizeF();
    m_foregroundOpacity = 100;
    m_underlines = false;
    m_adaptFontToSize = false;
    m_underlineLineSize = 1;
    m_lineSpacing = 1;
}

void TextItem::setContent(const QString &value)
{
    if (m_strText.compare(value)!=0){
        QString oldValue = m_strText;
        m_strText=value;
        if (allowHTML())
            m_text->setHtml(replaceReturns(value.trimmed()));
        else
            m_text->setPlainText(value);
        //m_text->setTextWidth(width());
        //m_textSize=m_text->size();
        if (itemMode() == DesignMode){
            initText();
        }

        if (!isLoading()){
            initText();
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
        initText();

    if (m_textSize.width()>width() && ((m_autoWidth==MaxWordLength)||(m_autoWidth==MaxStringLength))){
        setWidth(m_textSize.width() + fakeMarginSize()*2);
    }

    if ((m_textSize.height()>height()) && (m_autoHeight) ){
        setHeight(m_textSize.height()+borderLineSize()*2);
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

void TextItem::setTextFont(const QFont& value){
    m_text->setDefaultFont(value);
    if ((m_angle==Angle0)||(m_angle==Angle180)){
        m_text->setTextWidth(rect().width()-fakeMarginSize()*2);
    } else {
        m_text->setTextWidth(rect().height()-fakeMarginSize()*2);
    }
}

void TextItem::adaptFontSize(){
    QFont _font = transformToSceneFont(font());
    do{
        setTextFont(_font);
        if (_font.pixelSize()>2)
            _font.setPixelSize(_font.pixelSize()-1);
        else break;
    } while(m_text->size().height()>this->height() || m_text->size().width()>(this->width())-fakeMarginSize()*2);
}
int TextItem::underlineLineSize() const
{
    return m_underlineLineSize;
}

void TextItem::setUnderlineLineSize(int value)
{
    int oldValue = m_underlineLineSize;
    m_underlineLineSize = value;
    update();
    notify("underlineLineSize",oldValue,value);
}

int TextItem::lineSpacing() const
{
    return m_lineSpacing;
}

void TextItem::setLineSpacing(int value)
{
    int oldValue = m_lineSpacing;
    m_lineSpacing = value;
    initText();
    update();
    notify("lineSpacing",oldValue,value);
}


void TextItem::initText()
{
    QTextOption to;
    to.setAlignment(m_alignment);

    if (m_autoWidth!=MaxStringLength)
        if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth)))
            to.setWrapMode(QTextOption::WordWrap);
        else
            to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else to.setWrapMode(QTextOption::NoWrap);

    m_text->setDocumentMargin(0);
    m_text->setDefaultTextOption(to);

    QFont _font = transformToSceneFont(font());
    if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth))){
        adaptFontSize();
    } else {
        setTextFont(transformToSceneFont(font()));
    }
    
    if ((m_angle==Angle0)||(m_angle==Angle180)){
        m_text->setTextWidth(rect().width()-fakeMarginSize()*2);
    } else {
        m_text->setTextWidth(rect().height()-fakeMarginSize()*2);
    }
    
    for ( QTextBlock block = m_text->begin(); block.isValid(); block = block.next())
    {
        QTextCursor tc = QTextCursor(block);
        QTextBlockFormat fmt = block.blockFormat();

        if(fmt.lineHeight() != m_lineSpacing) {
            fmt.setLineHeight(m_lineSpacing,QTextBlockFormat::LineDistanceHeight);
            tc.setBlockFormat( fmt );
        }
    }

    m_textSize=m_text->size();
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
        if (m_text){
            if (allowHTML)
                m_text->setHtml(m_strText);
            else
                m_text->setPlainText(m_strText);
            update();
        }
        notify("allowHTML",!m_allowHTML,allowHTML);
    }
}
bool TextItem::trimValue() const
{
    return m_trimValue;
}

void TextItem::setTrimValue(bool value)
{
    bool oldValue = m_trimValue;
    m_trimValue = value;
    notify("trimValue",oldValue,value);
}


void TextItem::geometryChangedEvent(QRectF , QRectF)
{
//    if ((m_angle==Angle0)||(m_angle==Angle180)){
//        m_text->setTextWidth(rect().width()-fakeMarginSize()*2);
//    } else {
//        m_text->setTextWidth(rect().height()-fakeMarginSize()*2);
//    }
    if (itemMode() == DesignMode) initText();
    else if (adaptFontToSize()) initText();

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
            initText();
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
    setContent(context);
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

void TextItem::setAdaptFontToSize(bool value)
{
    if (m_adaptFontToSize!=value){
        bool oldValue = m_adaptFontToSize;
        m_adaptFontToSize=value;
        initText();
        invalidateRect(rect());
        notify("updateFontToSize",oldValue,value);
    }
}

bool TextItem::canBeSplitted(int height) const
{
    return height>(m_text->begin().layout()->lineAt(0).height());
}

BaseDesignIntf *TextItem::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    int linesHeight=0;
    initText();
    QString tmpText="";
    TextItem* upperPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));

    for (QTextBlock it=m_text->begin();it!=m_text->end();it=it.next()){
        for (int i=0;i<it.layout()->lineCount();i++){
          linesHeight+=it.layout()->lineAt(i).height()+lineSpacing();
          if (linesHeight>(height-(fakeMarginSize()*2+borderLineSize()*2))) {
              linesHeight-=it.layout()->lineAt(i).height(); goto loop_exit;
          }
          tmpText+=it.text().mid(it.layout()->lineAt(i).textStart(),it.layout()->lineAt(i).textLength())+'\n';
        }
    }
    loop_exit:
    tmpText = tmpText.trimmed();
    upperPart->setHeight(linesHeight+fakeMarginSize()*2+borderLineSize()*2);
    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    upperPart->setContent(context->extendTextByTags(tmpText,0));
    return upperPart;
}

BaseDesignIntf *TextItem::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem* bottomPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    int linesHeight=0;
    int curLine=0;
    QTextBlock curBlock;

    QString tmpText="";

    for (curBlock=m_text->begin();curBlock!=m_text->end();curBlock=curBlock.next()){
        for (curLine=0;curLine<curBlock.layout()->lineCount();curLine++){
            linesHeight+=curBlock.layout()->lineAt(curLine).height()+lineSpacing();
            if (linesHeight>(height-(fakeMarginSize()*2+borderLineSize()*2))) {goto loop_exit;}
        }
    }
    loop_exit:;
    int textPos=0;
    for (;curBlock!=m_text->end();curBlock=curBlock.next()){
        for (;curLine<curBlock.layout()->lineCount();curLine++){
            if (tmpText=="") textPos= curBlock.layout()->lineAt(curLine).textStart();
            tmpText+=curBlock.text().mid(curBlock.layout()->lineAt(curLine).textStart(),
              curBlock.layout()->lineAt(curLine).textLength()) +"\n";
        }
    }

    if (!m_strText.endsWith("\n")) tmpText = tmpText.trimmed();

    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    bottomPart->setContent(context->extendTextByTags(tmpText,textPos));
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
        initText();
    }
}

void TextItem::setTextItemFont(QFont value)
{
    if (font()!=value){
        QFont oldValue = font();
        setFont(value);
        m_text->setDefaultFont(transformToSceneFont(value));
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
            initText();
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

void TextItem::setUnderlines(bool value)
{
    if (m_underlines != value){
        bool oldValue = m_underlines;
        m_underlines = value;
        update();
        notify("underlines",oldValue,value);
    }
}

} //namespace LimeReport



