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
#include "lrxmlbasetypesserializators.h"
#include "serializators/lrxmlserializatorsfactory.h"
#include "lrsimplecrypt.h"
#include <QFont>
#include <QImage>
#include <QColor>

namespace{

LimeReport::SerializatorIntf * createIntSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlIntSerializator(doc,node);
}

LimeReport::SerializatorIntf * createQRealSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQRealSerializator(doc,node);
}

LimeReport::SerializatorIntf * createQStringSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQStringSerializator(doc,node);
}

LimeReport::SerializatorIntf * createEnumAndFlagsSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlEnumAndFlagsSerializator(doc,node);
}

LimeReport::SerializatorIntf * createBoolSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlBoolSerializator(doc,node);
}

LimeReport::SerializatorIntf * createFontSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlFontSerializator(doc,node);
}

LimeReport::SerializatorIntf * createQSizeFSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQSizeFSerializator(doc,node);
}

LimeReport::SerializatorIntf * createQImageSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQImageSerializator(doc,node);
}

LimeReport::SerializatorIntf * createQColorSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlColorSerializator(doc,node);
}

LimeReport::SerializatorIntf* createQByteArraySerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XmlQByteArraySerializator(doc,node);
}

bool registredQString = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QString", createQStringSerializator);
bool registredInt = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("int", createIntSerializator);
bool registredEnumAndFlags = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("enumAndFlags",createEnumAndFlagsSerializator);
bool registredBool = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("bool", createBoolSerializator);
bool registredQFont = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QFont", createFontSerializator);
bool registredQSizeF = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QSizeF", createQSizeFSerializator);
bool registredQImage = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QImage", createQImageSerializator);
bool registredQReal = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("qreal", createQRealSerializator);
bool registerDouble = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("double", createQRealSerializator);
bool registerQColor = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QColor", createQColorSerializator);
bool registerQByteArray = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QByteArray", createQByteArraySerializator);

}


namespace LimeReport{

void XmlBaseSerializator::saveBool(QDomElement node,QString name, bool value)
{
    if (value) node.setAttribute(name,1);
    else node.setAttribute(name,0);
}

void XmlQStringSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QString");
    if (name.compare("password")==0){
        Chipper chipper;
        QByteArray ba = chipper.cryptString(value.toString());
        //ba.append();
        _node.setAttribute("Value",QString(ba.toBase64()));
    } else {
        _node.appendChild(doc()->createTextNode(value.toString()));
    }
    node()->appendChild(_node);
}

QVariant XmlQStringSerializator::loadValue()
{
    if (node()->tagName().compare("password")==0){
        QByteArray ba;
        Chipper chipper;
        ba.append(node()->attribute("Value").toLatin1());
        return chipper.decryptByteArray(QByteArray::fromBase64(ba));
    } else
        return node()->text();
}

void XmlIntSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","int");
    _node.setAttribute("Value",value.toInt());
    node()->appendChild(_node);
}

QVariant XmlIntSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlEnumAndFlagsSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","enumAndFlags");
    _node.setAttribute("Value",value.toInt());
    node()->appendChild(_node);
}

QVariant XmlEnumAndFlagsSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlBoolSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","bool");
    if (value.toBool())
      _node.setAttribute("Value",1);
    else _node.setAttribute("Value",0);

    node()->appendChild(_node);
}

QVariant XmlBoolSerializator::loadValue()
{
    return node()->attribute("Value").toInt();
}

void XmlFontSerializator::save(const QVariant &value, QString name)
{
    QFont font = value.value<QFont>();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QFont");
    _node.setAttribute("family",font.family());
    _node.setAttribute("pointSize",font.pointSize());
    _node.setAttribute("stylename",font.styleName());
    _node.setAttribute("weight",font.weight());
    //saveBool(_node,"bold",font.bold());
    saveBool(_node,"italic",font.italic());
    saveBool(_node,"underline",font.underline());
    node()->appendChild(_node);
}

QVariant XmlFontSerializator::loadValue()
{
    QFont font;
    font.setFamily(node()->attribute("family"));
    font.setPointSize(node()->attribute("pointSize").toInt());
    font.setStyleName(node()->attribute("stylename"));
    font.setWeight(node()->attribute("weight").toInt());
    if (!node()->attribute("bold").isEmpty())
      font.setBold(node()->attribute("bold").toInt());
    font.setItalic(node()->attribute("italic").toInt());
    font.setUnderline(node()->attribute("underline").toInt());
    return font;
}

void XmlQSizeFSerializator::save(const QVariant &value, QString name)
{
    QSizeF size = value.toSizeF();
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QSizeF");
    _node.setAttribute("width",QString::number(size.width()));
    _node.setAttribute("height",QString::number(size.height()));
    node()->appendChild(_node);
}

QVariant XmlQSizeFSerializator::loadValue()
{
    QSizeF size;
    size.setWidth(node()->attribute("width").toDouble());
    size.setHeight(node()->attribute("height").toDouble());
    return size;
}

void XmlQImageSerializator::save(const QVariant &value, QString name)
{
    QImage image=value.value<QImage>();
    QByteArray ba;
    QBuffer buff(&ba);
    buff.open(QIODevice::WriteOnly);
    image.save(&buff,"PNG");
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QImage");
    _node.appendChild(doc()->createTextNode(ba.toHex()));
    node()->appendChild(_node);
}

QVariant XmlQImageSerializator::loadValue()
{
    QImage img;
    img.loadFromData(QByteArray::fromHex(node()->text().toLatin1()),"PNG");
    return img;
}

void XmlQRealSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","qreal");
    _node.setAttribute("Value",QString::number(value.toDouble()));
    node()->appendChild(_node);
}

QVariant XmlQRealSerializator::loadValue()
{
    return node()->attribute("Value").toDouble();
}

void XmlColorSerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QColor");
    _node.setAttribute("Value",value.value<QColor>().name());
    node()->appendChild(_node);
}

QVariant XmlColorSerializator::loadValue()
{
    return QColor(node()->attribute("Value"));
}

void XmlQByteArraySerializator::save(const QVariant &value, QString name)
{
    QDomElement _node = doc()->createElement(name);
    _node.setAttribute("Type","QByteArray");
    _node.setAttribute("Value",QString(value.toByteArray().toBase64()));
    node()->appendChild(_node);
}

QVariant XmlQByteArraySerializator::loadValue()
{
    QByteArray ba;
    ba.append(node()->attribute("Value").toLatin1());
    return QByteArray::fromBase64(ba);
}



}



