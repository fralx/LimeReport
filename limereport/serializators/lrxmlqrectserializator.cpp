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
#include "lrxmlqrectserializator.h"
#include "lrserializatorintf.h"
#include "lrxmlserializatorsfactory.h"

#include <QRect>
#include <QRectF>

namespace{

LimeReport::SerializatorIntf * createQRectSerializator(QDomDocument *doc, QDomElement *node){
    return new LimeReport::XMLQRectSerializator(doc,node);
}

//LimeReport::SerializatorIntf * createQRectFSerializator(QDomDocument *doc, QDomElement *node){
//    return new LimeReport::XMLQRectFSerializator(doc,node);
//}

bool VARIABLE_IS_NOT_USED registredQRect = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QRect", createQRectSerializator);
bool VARIABLE_IS_NOT_USED registredQRectF = LimeReport::XMLAbstractSerializatorFactory::instance().registerCreator("QRectF", createQRectSerializator);

}

QVariant LimeReport::XMLQRectSerializator::loadValue()
{
    QRect rect;
    //QDomElement geometryNode = m_node->firstChildElement(name);
    //if (!geometryNode.isNull()){
        rect = QRect(
                    m_node->attribute("x").toInt(),
                    m_node->attribute("y").toInt(),
                    m_node->attribute("width").toInt(),
                    m_node->attribute("height").toInt()
        );
    //}
    return rect;
}

void LimeReport::XMLQRectSerializator::save(const QVariant &value, QString name)
{
    QDomElement geometryNode = m_doc->createElement(name);
    geometryNode.setAttribute("Type","QRect");
    geometryNode.setAttribute("x",value.toRect().x());
    geometryNode.setAttribute("x",value.toRect().x());
    geometryNode.setAttribute("y",value.toRect().y());
    geometryNode.setAttribute("width",value.toRect().width());
    geometryNode.setAttribute("height",value.toRect().height());
    m_node->appendChild(geometryNode);
}

void LimeReport::XMLQRectFSerializator::save(const QVariant &value, QString name)
{
    QDomElement geometryNode = m_doc->createElement(name);
    geometryNode.setAttribute("Type","QRectF");
    geometryNode.setAttribute("x",value.toRect().x());
    geometryNode.setAttribute("y",value.toRect().y());
    geometryNode.setAttribute("width",value.toRect().width());
    geometryNode.setAttribute("height",value.toRect().height());
    m_node->appendChild(geometryNode);
}

QVariant LimeReport::XMLQRectFSerializator::loadValue()
{
    QRectF rect;
    //QDomElement geometryNode = m_node->firstChildElement(name);
    //if (!geometryNode.isNull()){
        rect = QRect(
                    m_node->attribute("x").toDouble(),
                    m_node->attribute("y").toDouble(),
                    m_node->attribute("width").toDouble(),
                    m_node->attribute("height").toDouble()
        );

    //}
    return rect;
}
