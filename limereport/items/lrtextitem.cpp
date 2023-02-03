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
#include <QtGui>
#include <QTextLayout>
#include <QLocale>
#include <QMessageBox>
#include <math.h>

#include "lrpagedesignintf.h"
#include "lrtextitem.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"
#include "lrsimpletagparser.h"
#include "lrtextitemeditor.h"
#include "lrreportengine_p.h"
#include <QMenu>

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
      m_allowHTMLInFields(false), m_replaceCarriageReturns(false), m_followTo(""), m_follower(0), m_textIndent(0),
      m_textLayoutDirection(Qt::LayoutDirectionAuto), m_hideIfEmpty(false), m_fontLetterSpacing(0)
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

TextItem::~TextItem(){}

int TextItem::fakeMarginSize() const{
    return marginSize()+5;
}

void TextItem::preparePopUpMenu(QMenu &menu)
{
    QAction* editAction = menu.addAction(QIcon(":/report/images/edit_pecil2.png"),tr("Edit"));
    menu.insertAction(menu.actions().at(0),editAction);
    menu.insertSeparator(menu.actions().at(1));

    menu.addSeparator();

    QAction* action = menu.addAction(tr("Auto height"));
    action->setCheckable(true);
    action->setChecked(autoHeight());

    action = menu.addAction(tr("Allow HTML"));
    action->setCheckable(true);
    action->setChecked(allowHTML());

    action = menu.addAction(tr("Allow HTML in fields"));
    action->setCheckable(true);
    action->setChecked(allowHTMLInFields());

    action = menu.addAction(tr("Stretch to max height"));
    action->setCheckable(true);
    action->setChecked(stretchToMaxHeight());

    action = menu.addAction(tr("Transparent"));
    action->setCheckable(true);
    action->setChecked(backgroundMode() == TransparentMode);

    action = menu.addAction(tr("Watermark"));
    action->setCheckable(true);
    action->setChecked(isWatermark());

    action = menu.addAction(tr("Hide if empty"));
    action->setCheckable(true);
    action->setChecked(hideIfEmpty());

}

void TextItem::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Edit")) == 0){
        this->showEditorDialog();
    }
    if (page()){
        if (action->text().compare(tr("Auto height")) == 0){
            page()->setPropertyToSelectedItems("autoHeight",action->isChecked());
        }
        if (action->text().compare(tr("Allow HTML")) == 0){
            page()->setPropertyToSelectedItems("allowHTML",action->isChecked());
        }
        if (action->text().compare(tr("Allow HTML in fields")) == 0){
            page()->setPropertyToSelectedItems("allowHTMLInFields",action->isChecked());
        }
        if (action->text().compare(tr("Stretch to max height")) == 0){
            page()->setPropertyToSelectedItems("stretchToMaxHeight",action->isChecked());
        }
    }
    if (action->text().compare(tr("Transparent")) == 0){
        if (action->isChecked()){
            setProperty("backgroundMode",TransparentMode);
        } else {
            setProperty("backgroundMode",OpaqueMode);
        }
    }
    if (action->text().compare(tr("Watermark")) == 0){
        page()->setPropertyToSelectedItems("watermark",action->isChecked());
    }

    if (action->text().compare(tr("Hide if empty")) == 0){
        page()->setPropertyToSelectedItems("hideIfEmpty",action->isChecked());
    }

    ContentItemDesignIntf::processPopUpAction(action);
}

void TextItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
    Q_UNUSED(widget);
    Q_UNUSED(style);


    TextPtr text = textDocument();

    painter->save();

    setupPainter(painter);
    prepareRect(painter,style,widget);

    QSizeF tmpSize = rect().size()-text->size();

    if (!painter->clipRegion().isEmpty()){
        QRegion clipReg=painter->clipRegion().xored(painter->clipRegion().subtracted(rect().toRect()));
        painter->setClipRegion(clipReg);
    } else {
        painter->setClipRect(rect());
    }

    qreal hOffset = 0, vOffset = 0;
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
            hOffset = width() - fakeMarginSize();
            vOffset = height() - fakeMarginSize();
            if ((tmpSize.width()>0) && (m_alignment & Qt::AlignVCenter)){
                vOffset = tmpSize.height() / 2+ text->size().height();
            }
            if ((tmpSize.height()>0) && (m_alignment & Qt::AlignBottom)){
                vOffset = (text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(180);
        break;
        case Angle270:
            hOffset = fakeMarginSize();
            vOffset = height()-fakeMarginSize();
            if (m_alignment & Qt::AlignVCenter){
                hOffset = (width() - text->size().height())/2;
            }

            if (m_alignment & Qt::AlignBottom){
                hOffset = (width() - text->size().height());
            }
            painter->translate(hOffset,vOffset);
            painter->rotate(270);
        break;
        case Angle45:
            painter->translate(width()/2,0);
            painter->rotate(45);
            text->setTextWidth(sqrt(2*(pow(width()/2,2))));
        break;
        case Angle315:
            painter->translate(0,height()/2);
            painter->rotate(315);
            text->setTextWidth(sqrt(2*(pow(height()/2,2))));
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

    for(QTextBlock it = text->begin(); it != text->end(); it=it.next()){
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

    text->documentLayout()->draw(painter,ctx);

    if (m_underlines){
        if (lineHeight<0) lineHeight = painter->fontMetrics().height();
        for (curpos+=lineHeight; curpos<rect().height();curpos+=lineHeight){
            painter->drawLine(QPointF(0,curpos),QPoint(rect().width(),curpos));
        }
    }

    painter->restore();
    BaseDesignIntf::paint(painter, style, widget);
}

void TextItem::Init()
{
    m_autoWidth = NoneAutoWidth;
    m_alignment = Qt::AlignLeft|Qt::AlignTop;
    m_autoHeight = false;
    m_textSize = QSizeF();
    m_firstLineSize = 0;
    m_foregroundOpacity = 100;
    m_underlines = false;
    m_adaptFontToSize = false;
    m_underlineLineSize = 1;
    m_lineSpacing = 1;
    m_valueType = Default;
}

void TextItem::setContent(const QString &value)
{
    if (m_strText.compare(value)!=0){
        QString oldValue = m_strText;

        m_strText = value;

        if (!isLoading()){
            if (autoHeight() || autoWidth() || hasFollower())
                initTextSizes();
            update(rect());
            notify("content",oldValue,value);
        }
    }
}

QString TextItem::content() const{
    return m_strText;
}

void TextItem::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{

    if (isNeedExpandContent())
        expandContent(dataManager, pass);

    if (!isLoading() && (autoHeight() || autoWidth() || hasFollower()) )
        initTextSizes();

    if (m_textSize.width()>width() && ((m_autoWidth==MaxWordLength)||(m_autoWidth==MaxStringLength))){
        setWidth(m_textSize.width() + fakeMarginSize()*2);
    }

    if (m_textSize.height()>height()) {
        if (m_autoHeight)
            setHeight(m_textSize.height()+borderLineSize()*2);
        else if (hasFollower() && !content().isEmpty()){
            follower()->setContent(getTextPart(0,height()));
            setContent(getTextPart(height(),0));
        }
    }
    BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
    if (isEmpty() && hideIfEmpty()) setVisible(false);
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
    QRegExp rx("$*\\{[^{]*\\}");
#else
    bool result = false;
    QRegularExpression rx("\\$*\\{[^{]*\\}");
    result = content().contains(rx) || isContentBackedUp();
    return result;
#endif
    return content().contains(rx) || isContentBackedUp();
}

QString TextItem::replaceBR(QString text) const
{
    return text.replace("<br/>","\n");
}

QString TextItem::replaceReturns(QString text) const
{
    QString result = text.replace("\r\n","<br/>");
    result = result.replace("\n","<br/>");
    return result;
}

void TextItem::setTextFont(TextPtr text, const QFont& value) const {
    text->setDefaultFont(value);
    if ((m_angle==Angle0)||(m_angle==Angle180)){
        text->setTextWidth(rect().width()-fakeMarginSize()*2);
    } else {
        text->setTextWidth(rect().height()-fakeMarginSize()*2);
    }
}

void TextItem::adaptFontSize(TextPtr text) const{
    QFont _font = transformToSceneFont(font());
    do{
//        qApp->processEvents();
        setTextFont(text,_font);
        if (_font.pixelSize()>2)
            _font.setPixelSize(_font.pixelSize()-1);
        else break;
    } while(text->size().height()>this->height() || text->size().width()>(this->width()) - fakeMarginSize() * 2);
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
//    if (autoHeight())
//        initTextSizes();
    update();
    notify("lineSpacing",oldValue,value);
}


void TextItem::initTextSizes() const
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
#if QT_VERSION < 0x050500
        str.sprintf(m_format.toStdString().c_str(), value);
#else
        str.asprintf(m_format.toStdString().c_str(), value);
#endif
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

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    switch (value.type()) {
        case QVariant::Date:
        case QVariant::DateTime:
            return formatDateTime(value.toDateTime());
        case QVariant::Double:
            return formatNumber(value.toDouble());
        default:
            return value.toString();
    }
#else
    switch (value.typeId()) {
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            return formatDateTime(value.toDateTime());
        case QMetaType::Double:
            return formatNumber(value.toDouble());
        default:
            return value.toString();
    }
#endif

}

TextItem::TextPtr TextItem::textDocument() const
{
    TextPtr text(new QTextDocument);
    QString content = m_trimValue ? m_strText.trimmed() : m_strText;

    if (allowHTML())
        if (isReplaceCarriageReturns()){
            text->setHtml(replaceReturns(content));
        } else {
            text->setHtml(content);
        }
    else
        text->setPlainText(content);

    QTextOption to;
    to.setAlignment(m_alignment);
    to.setTextDirection(m_textLayoutDirection);

    if (m_autoWidth!=MaxStringLength)
        if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth)))
            to.setWrapMode(QTextOption::WordWrap);
        else
            to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else to.setWrapMode(QTextOption::NoWrap);

    text->setDocumentMargin(0);
    text->setDefaultTextOption(to);

    QFont _font = transformToSceneFont(font());
    if (m_adaptFontToSize && (!(m_autoHeight || m_autoWidth))){
        adaptFontSize(text);
    } else {
        setTextFont(text,_font);
    }

    if (follower())
        text->documentLayout();

    if (m_lineSpacing != 1 || m_textIndent !=0 ){

        for ( QTextBlock block = text->begin(); block.isValid(); block = block.next())
        {
            QTextCursor tc = QTextCursor(block);
            QTextBlockFormat fmt = block.blockFormat();
            fmt.setTextIndent(m_textIndent);
            if (fmt.lineHeight() != m_lineSpacing) {
                fmt.setLineHeight(m_lineSpacing,QTextBlockFormat::LineDistanceHeight);
            }
            tc.setBlockFormat( fmt );
        }

    }

    return text;

}

int TextItem::fontLetterSpacing() const
{
    return m_fontLetterSpacing;
}

void TextItem::setFontLetterSpacing(int value)
{
    if (m_fontLetterSpacing != value){
        int oldValue = m_fontLetterSpacing;
        m_fontLetterSpacing = value;
        QFont curFont = font();
        curFont.setLetterSpacing(QFont::AbsoluteSpacing, m_fontLetterSpacing);
        setFont(curFont);
        notify("fontLetterSpacing", oldValue, value);
    }
}

bool TextItem::hideIfEmpty() const
{
    return m_hideIfEmpty;
}

void TextItem::setHideIfEmpty(bool hideEmpty)
{
    if (m_hideIfEmpty != hideEmpty){
        m_hideIfEmpty = hideEmpty;
        notify("hideIfEmpty",!m_hideIfEmpty, m_hideIfEmpty);
    }
}

bool TextItem::isReplaceCarriageReturns() const
{
    return m_replaceCarriageReturns;
}

void TextItem::setReplaceCarriageReturns(bool replaceCarriageReturns)
{
    if (replaceCarriageReturns != m_replaceCarriageReturns){
        m_replaceCarriageReturns = replaceCarriageReturns;
        update();
        notify("replaceCRwithBR",!replaceCarriageReturns, replaceCarriageReturns);
    }

}

qreal TextItem::textIndent() const
{
    return m_textIndent;
}

void TextItem::setTextIndent(const qreal &textIndent)
{
    if (m_textIndent != textIndent){
        qreal oldValue = m_textIndent;
        m_textIndent = textIndent;
        update();
        notify("textIndent", oldValue, textIndent);
    }
}

Qt::LayoutDirection TextItem::textLayoutDirection() const
{
    return m_textLayoutDirection;
}

void TextItem::setTextLayoutDirection(const Qt::LayoutDirection &textLayoutDirection)
{
    if (m_textLayoutDirection != textLayoutDirection){
        int oldValue = int(m_textLayoutDirection);
        m_textLayoutDirection = textLayoutDirection;
        update();
        notify("textLayoutDirection",oldValue,int(textLayoutDirection));
    }
}

void TextItem::setWatermark(bool watermark)
{
    if (watermark){
        setBackgroundMode(TransparentMode);
    }
    BaseDesignIntf::setWatermark(watermark);

}


QString TextItem::followTo() const
{
    return m_followTo;
}

void TextItem::setFollowTo(const QString &followTo)
{
    if (m_followTo != followTo){
        QString oldValue = m_followTo;
        m_followTo = followTo;
        if (!isLoading()){
            TextItem* fi = scene()->findChild<TextItem*>(oldValue);
            if (fi) fi->clearFollower();
            fi = scene()->findChild<TextItem*>(followTo);
            if (fi && fi != this){
                if (initFollower(followTo)){
                    notify("followTo",oldValue,followTo);
                } else {
                    m_followTo = "";
                    QMessageBox::critical(
                        0,
                        tr("Error"),
                        tr("TextItem \" %1 \" already has folower \" %2 \" ")
                            .arg(fi->objectName())
                            .arg(fi->follower()->objectName())
                    );
                    notify("followTo",followTo,"");
                }
            } else if (m_followTo != ""){
                QMessageBox::critical(
                    0,
                    tr("Error"),
                    tr("TextItem \" %1 \" not found!")
                        .arg(m_followTo)
                );
                notify("followTo",followTo,"");
            }
        }
    }
}

void TextItem::setFollower(TextItem *follower)
{
    if (!m_follower){
        m_follower = follower;
    }
}

void TextItem::clearFollower()
{
    m_follower = 0;
}

bool TextItem::hasFollower() const
{
    return m_follower != 0;
}

bool TextItem::initFollower(QString follower)
{
    TextItem* fi = scene()->findChild<TextItem*>(follower);
    if (fi){
        if (!fi->hasFollower()){
            fi->setFollower(this);
            return true;
        }
    }
    return false;
}

void TextItem::pageObjectHasBeenLoaded()
{
    if (!m_followTo.isEmpty()){
        initFollower(m_followTo);
    }
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
//        if (m_text){
//            if (allowHTML)
//                m_text->setHtml(m_strText);
//            else
//                m_text->setPlainText(m_strText);
//            update();
//        }
        update();
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
    update();
    notify("trimValue",oldValue,value);
}


void TextItem::geometryChangedEvent(QRectF , QRectF)
{}

bool TextItem::isNeedUpdateSize(RenderPass pass) const
{
    Q_UNUSED(pass)

    if ((autoHeight() || autoWidth()) || hasFollower()){
        initTextSizes();
    }

    bool res =  (m_textSize.height()>geometry().height()&&autoHeight()) ||
                (m_textSize.width()>geometry().width()&&autoWidth()) ||
                 m_follower ||
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
            update(rect());
            notify("alignment",QVariant(oldValue),QVariant(value));
        }
    }
}

void TextItem::expandContent(DataSourceManager* dataManager, RenderPass pass)
{
    QString context=content();
    foreach (QString variableName, dataManager->variableNamesByRenderPass(SecondPass)) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 1))
        QRegExp rx(QString(Const::NAMED_VARIABLE_RX).arg(variableName));
#else
        QRegularExpression rx = getNamedVariableRegEx(variableName);
#endif
        if (context.contains(rx) && pass == FirstPass){
            backupContent();
            break;
        }
    }

    ExpandType expandType = (allowHTML() && !allowHTMLInFields()) ? ReplaceHTMLSymbols : NoEscapeSymbols;
    switch(pass){
    case FirstPass:
        if (!fillInSecondPass()){
            context=expandUserVariables(context, pass, expandType, dataManager);
            context=expandScripts(context, dataManager);
            context=expandDataFields(context, expandType, dataManager);
        } else {
            context=expandDataFields(context, expandType, dataManager);
        }
        break;
    case SecondPass:
        if (isContentBackedUp()) {
            restoreContent();
            context = content();
        }
        context=expandUserVariables(context, pass, expandType, dataManager);
        context=expandScripts(context, dataManager);
    }

    if (expandType == NoEscapeSymbols && !m_varValue.isNull() &&m_valueType != Default) {
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

void TextItem::setAdaptFontToSize(bool value)
{
    if (m_adaptFontToSize!=value){
        bool oldValue = m_adaptFontToSize;
        m_adaptFontToSize=value;
//        initText();
        invalidateRect(rect());
        notify("updateFontToSize",oldValue,value);
    }
}

bool TextItem::canBeSplitted(int height) const
{
    QFontMetrics fm(font());
    return height > m_firstLineSize;
}

QString TextItem::extractText(QTextBlock& curBlock, int height){
    int curLine = 0;
    int linesHeight = 0;
    QString resultText;
    for (;curBlock != curBlock.document()->end() || curLine<=curBlock.lineCount(); curBlock = curBlock.next(), curLine = 0, resultText += '\n' ){
        linesHeight+=curBlock.blockFormat().topMargin();
        for (;curLine < curBlock.layout()->lineCount(); curLine++){
            linesHeight += curBlock.layout()->lineAt(curLine).height() + lineSpacing();
            if (height > 0 && linesHeight > (height-borderLineSize() * 2)) {goto loop_exit;}
            resultText += curBlock.text().mid(curBlock.layout()->lineAt(curLine).textStart(),
                                                   curBlock.layout()->lineAt(curLine).textLength());
        }
    }
    loop_exit: return resultText;
}

QString TextItem::getTextPart(int height, int skipHeight){

    QString resultText = "";
    TextPtr text = textDocument();
    text->size().height();
    QTextBlock curBlock = text->begin();
    QTextCursor cursor(text.data());
    cursor.movePosition(QTextCursor::Start);

    if (skipHeight > 0){
        resultText = extractText(curBlock, skipHeight);
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, resultText.length());
    }

    resultText = extractText(curBlock, height);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, resultText.length());

    if (allowHTML()){
        resultText = cursor.selection().toHtml();
        resultText.remove("<!--StartFragment-->");
        resultText.remove("<!--EndFragment-->");
    } else {
        resultText = cursor.selection().toPlainText();
    }

    return resultText;
}

void TextItem::restoreLinksEvent()
{
    if (!followTo().isEmpty()){
        BaseDesignIntf* pi = dynamic_cast<BaseDesignIntf*>(parentItem());
        if (pi){
            foreach (BaseDesignIntf* bi, pi->childBaseItems()) {
                if (bi->patternName().compare(followTo())==0){
                    TextItem* ti = dynamic_cast<TextItem*>(bi);
                    if (ti){
                        ti->setFollower(this);
                    }
                }
            }
        }
    }
}

BaseDesignIntf *TextItem::cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem* upperPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    upperPart->setContent(getTextPart(height,0));
    upperPart->initTextSizes();
    upperPart->setHeight(upperPart->textSize().height()+borderLineSize()*2);
    return upperPart;
}

BaseDesignIntf *TextItem::cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem* bottomPart = dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    bottomPart->setContent(getTextPart(0,height));
    bottomPart->initTextSizes();
    bottomPart->setHeight(bottomPart->textSize().height()+borderLineSize()*2);
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
//    if (itemMode() == DesignMode || !isNeedExpandContent()){
//        if (autoHeight() && autoWidth())
//            initTextSizes();
//    }
}

void TextItem::setTextItemFont(QFont value)
{
    if (font()!=value){
        QFont oldValue = font();
        value.setLetterSpacing(QFont::AbsoluteSpacing, m_fontLetterSpacing);
        setFont(value);
        if (!isLoading()) update();
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



