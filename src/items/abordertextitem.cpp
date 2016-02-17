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

#include "apagedesignintf.h"
#include "abordertextitem.h"
#include "adesignelementsfactory.h"
#include "aglobal.h"
#include "adatasourcemanager.h"
#include "asimpletagparser.h"

namespace{

const QString xmlTag = "TextItem";

ArReport::BaseDesignIntf * createTextItem(QObject* owner, ArReport::BaseDesignIntf*  parent){
    return new ArReport::TextItem(owner,parent);
}
bool registred = ArReport::DesignElementsFactory::instance().registerCreator(xmlTag, ArReport::ItemAttribs(QObject::tr("Text Item"),"TextItem"), createTextItem);

}

namespace ArReport{

TextItem::TextItem(QObject *owner, QGraphicsItem *parent)
    : ItemDesignIntf(xmlTag,owner,parent),m_spaceBorder(4){
    m_text = new QTextDocument();
    Init();
}

TextItem::TextItem(const QString& text, QObject* owner /*= 0*/, QGraphicsItem* parent /*= 0*/)
    :ItemDesignIntf(xmlTag, owner,parent),m_spaceBorder(4){
    m_strText = text;
    m_text = new QTextDocument(text);
    m_text->setHtml(m_strText);
    initText();
    Init();
}

TextItem::~TextItem()
{
    delete m_text;
}

void TextItem::paint(QPainter* ppainter, const QStyleOptionGraphicsItem* style, QWidget* widget) {
    Q_UNUSED(widget);
    Q_UNUSED(style);

    ppainter->save();

    setupPainter(ppainter);

    if (isSelected()) {
        ppainter->setOpacity(0);
        ppainter->fillRect(rect(),QBrush(QColor(Qt::white)));
        ppainter->setOpacity(1);
    }
    else ppainter->fillRect(rect(),QBrush(Qt::white));

    //initText();

    QSizeF tmpSize = rect().size()-m_textSize;//m_text->size();

    if ((tmpSize.height()>0) && (m_alignment & Qt::AlignVCenter))
      ppainter->translate(0,tmpSize.height()/2);

    QRegion clipReg=ppainter->clipRegion().xored(ppainter->clipRegion().subtracted(rect().toRect()));
    ppainter->setClipRegion(clipReg);
    qreal pos=0;
    for(QTextBlock it=m_text->begin();it!=m_text->end();it=it.next()){
        for (int i=0;i<it.layout()->lineCount();i++){
            pos+=it.layout()->lineAt(i).height();
            it.layout()->lineAt(i).draw(ppainter,QPointF(spaceBorderSize(),0));
        }
    }

    ppainter->restore();
    BaseDesignIntf::paint(ppainter, style, widget);
}

QString TextItem::text() const{
    return m_strText;
}

void TextItem::Init()
{
    //setFlag(QGraphicsItem::ItemIsMovable);
    //setFlag(QGraphicsItem::ItemIsSelectable);
    m_autoWidth=NoneAutoWidth;
    m_alignment= Qt::AlignLeft|Qt::AlignTop;
    m_autoHeight=false;
    m_text->setDefaultFont(sceneFont());
    m_textSize=QSizeF();
}

void TextItem::setText(QString value)
{
    if (m_strText.compare(value)!=0){
        m_strText=value;
        m_text->setHtml(value);
        m_text->setTextWidth(width());
        m_textSize=m_text->size();
        if (!isLoading()){
          update(rect());
        }
    }
}

void TextItem::updateItemSize(RenderPass pass, int /*maxHeight*/)
{
    if (isNeedExpandContent())
        expandContent(pass);
    initText();

    if (m_textSize.width()>width() && (m_autoWidth==MaxWordLength)){
        setWidth(m_textSize.width());
    }
    if ((m_textSize.height()>height()) && (m_autoHeight) ){
        setHeight(m_textSize.height()+5);
    }
}

void TextItem::updateLayout()
{
    m_layout.setText(text());
    qreal linePos = 0;
    m_layout.beginLayout();
    while(true){
        QTextLine line = m_layout.createLine();
        if (!line.isValid()) break;
        line.setLineWidth(width()-m_spaceBorder*2);
        line.setPosition(QPoint(m_spaceBorder,linePos));
        linePos+=line.height();
    }
    m_layout.endLayout();
}

bool TextItem::isNeedExpandContent() const
{
    QRegExp rx("$*\\{[^{]*\\}");
    return text().contains(rx);
}

QString TextItem::expandDataFields(QString context)
{
    DataManager* dm=DataManager::instance();
    QRegExp rx("\\$D\\{[^\\{]*\\}");
    if (context.contains(rx)){

        while ((rx.indexIn(context))!=-1){
            QString field=rx.cap(0);
            field.remove("$D{");
            field.remove("}");
            if (dm->isContainField(field)) {
                context.replace(rx.cap(0),dm->fieldData(field).toString());
            } else {
                context.replace(rx.cap(0),tr("Field %1 not found").arg(field));
            }
        }
    }
    return context;
}

void TextItem::initText()
{
    QTextOption to;
    to.setAlignment(m_alignment);
    to.setWrapMode(QTextOption::WordWrap);
    m_text->setDocumentMargin(spaceBorderSize());
    m_text->setDefaultTextOption(to);
    m_text->setDefaultFont(sceneFont());
    m_text->setTextWidth(rect().width());
    m_textSize=m_text->size();
}

void TextItem::setGeometry(QRectF rect)
{
    BaseDesignIntf::setGeometry(rect);
    m_text->setTextWidth(width());
}

bool TextItem::isNeedUpdateSize() const
{
    return  ((m_textSize.height()>geometry().height()||m_textSize.width()>geometry().width())&&
             (autoHeight()||autoWidth()))
            || isNeedExpandContent();
}

void TextItem::setSpaceBorderSize(int value)
{
    m_spaceBorder=value;
    update(rect());
}

void TextItem::setAlignment(Qt::Alignment value)
{
    m_alignment=value;
    m_layout.setTextOption(QTextOption(m_alignment));
    update(rect());
}

void TextItem::expandContent(RenderPass pass)
{
    QString context=text();
    switch(pass){
    case FirstPass:
        context=expandDataFields(context);
        break;
    case SecondPass:;
    }
    setText(context);
}

bool TextItem::canBeSliced(int height) const
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
          linesHeight+=it.layout()->lineAt(i).height();
          if ((linesHeight>height)) {linesHeight-=it.layout()->lineAt(i).height(); goto loop_exit;}
          tmpText+=it.text().mid(it.layout()->lineAt(i).textStart(),it.layout()->lineAt(i).textLength());
        }
    }
    loop_exit:
    upperPart->setHeight(linesHeight);
    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    upperPart->setText(context->extendTextByTags(tmpText,0));
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
            linesHeight+=curBlock.layout()->lineAt(curLine).height();
            if (linesHeight>height) {goto loop_exit;}
        }
    }
    loop_exit:;
    int textPos=0;
    for (;curBlock!=m_text->end();curBlock=curBlock.next()){
        for (;curLine<curBlock.layout()->lineCount();curLine++){
            if (tmpText=="") textPos= curBlock.layout()->lineAt(curLine).textStart();
            tmpText+=curBlock.text().mid(curBlock.layout()->lineAt(curLine).textStart(),
                                                   curBlock.layout()->lineAt(curLine).textLength());
        }
    }
    QScopedPointer<HtmlContext> context(new HtmlContext(m_strText));
    bottomPart->setText(context->extendTextByTags(tmpText,textPos));
    return bottomPart;
}

BaseDesignIntf *TextItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new TextItem(owner,parent);
}

BaseDesignIntf *TextItem::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{
    TextItem* empty=dynamic_cast<TextItem*>(cloneItem(itemMode(),owner,parent));
    empty->setText("");
    empty->setHeight(height);
    return empty;
}

void TextItem::objectLoadFinished()
{
    ItemDesignIntf::objectLoadFinished();
    initText();
}

void TextItem::setTextItemFont(QFont value)
{
    setFont(value);
    m_text->setDefaultFont(sceneFont());
}

}

