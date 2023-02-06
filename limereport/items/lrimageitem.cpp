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
#include "lrimageitem.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"
#include "lrpagedesignintf.h"
#include "lrimageitemeditor.h"

namespace{

const QString xmlTag = "ImageItem";

LimeReport::BaseDesignIntf * createImageItem(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::ImageItem(owner,parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
                     xmlTag, LimeReport::ItemAttribs(QObject::tr("Image Item"),"Item"), createImageItem
                 );
}

namespace LimeReport{

ImageItem::ImageItem(QObject* owner,QGraphicsItem* parent)
    :ItemDesignIntf(xmlTag,owner,parent), m_useExternalPainter(false), m_externalPainter(0),
     m_autoSize(false), m_scale(true),
     m_keepAspectRatio(true), m_center(true), m_format(Binary){}

BaseDesignIntf *ImageItem::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    ImageItem* result = new ImageItem(owner,parent);
    result->setExternalPainter(m_externalPainter);
    return result;
}

void ImageItem::loadPictureFromVariant(QVariant& data){
    //TODO: Migrate to QMetaType
    if (data.isValid()){
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (data.typeId() == QMetaType::QImage){
#else
        if (data.type() == QVariant::Image){
#endif
          m_picture =  data.value<QImage>();
        } else {
            switch (m_format) {
            default:
            case Binary:
                m_picture.loadFromData(data.toByteArray());
                break;
            case Hex:
                m_picture.loadFromData(QByteArray::fromHex(data.toByteArray()));
                break;
            case Base64:
                m_picture.loadFromData(QByteArray::fromBase64(data.toByteArray()));
                break;
            }
        }

    }
}

void ImageItem::preparePopUpMenu(QMenu &menu)
{
    QAction* editAction = menu.addAction(QIcon(":/report/images/edit_pecil2.png"),tr("Edit"));
    menu.insertAction(menu.actions().at(0),editAction);
    menu.insertSeparator(menu.actions().at(1));

    menu.addSeparator();
    QAction* action = menu.addAction(tr("Watermark"));
    action->setCheckable(true);
    action->setChecked(isWatermark());

}

void ImageItem::processPopUpAction(QAction *action)
{
    if (action->text().compare(tr("Watermark")) == 0){
        page()->setPropertyToSelectedItems("watermark",action->isChecked());
    }
    if (action->text().compare(tr("Edit")) == 0){
        this->showEditorDialog();
    }
    ItemDesignIntf::processPopUpAction(action);
}

QImage getFileByResourcePath(QString resourcePath) {
    QFileInfo resourceFile(resourcePath);
    if (resourceFile.exists())
        return QImage(resourcePath);
    return QImage();
}

QImage ImageItem::drawImage() const
{
    if (image().isNull())
        return getFileByResourcePath(m_resourcePath);
    return image();
}

bool ImageItem::useExternalPainter() const
{
    return m_useExternalPainter;
}

void ImageItem::setUseExternalPainter(bool value)
{
    if (m_useExternalPainter != value){
        m_useExternalPainter = value;
        notify("useExternalPainter",!value, value);
        update();
    }
}

QWidget *ImageItem::defaultEditor()
{
    ImageItemEditor* editor = new ImageItemEditor(this);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    return editor;
}

QByteArray ImageItem::imageAsByteArray() const
{
    QByteArray result;
    QBuffer buffer(&result);
    buffer.open(QIODevice::WriteOnly);
    m_picture.save(&buffer,"PNG");

    return result;
}

void ImageItem::setImageAsByteArray(QByteArray image)
{
    QImage value;
    value.loadFromData(image);
    setImage(value);
}

QString ImageItem::fileFilter() const
{
    return tr("Images (*.gif *.icns *.ico *.jpeg *.tga *.tiff *.wbmp *.webp *.png *.jpg *.bmp);;All(*.*)");
}

void ImageItem::updateItemSize(DataSourceManager* dataManager, RenderPass pass, int maxHeight)
{

    if (m_picture.isNull()){
        if (!m_datasource.isEmpty() && !m_field.isEmpty()){
            IDataSource* ds = dataManager->dataSource(m_datasource);
           if (ds) {
              QVariant data = ds->data(m_field);
              loadPictureFromVariant(data);
           }
       } else if (!m_resourcePath.isEmpty()){
           m_resourcePath = expandUserVariables(m_resourcePath, pass, NoEscapeSymbols, dataManager);
           m_resourcePath = expandDataFields(m_resourcePath, NoEscapeSymbols, dataManager);
           m_picture = QImage(m_resourcePath);
       } else if (!m_variable.isEmpty()){
            //TODO: Migrate to QMetaType
           QVariant data = dataManager->variable(m_variable);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
           if (data.typeId() == QMetaType::QString){
#else
           if (data.type() == QVariant::String){
#endif
                m_picture = QImage(data.toString());
           } else {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
               if (data.typeId() == QMetaType::QImage){
#else
               if (data.type() == QVariant::Image){
#endif
                    loadPictureFromVariant(data);
               }
           }
       }
   }
   if (m_autoSize){
       setWidth(m_picture.width());
       setHeight(m_picture.height());
   }
   BaseDesignIntf::updateItemSize(dataManager, pass, maxHeight);
}

bool ImageItem::isNeedUpdateSize(RenderPass) const
{
    return m_picture.isNull() || m_autoSize;
}

QString ImageItem::resourcePath() const
{
    return m_resourcePath;
}

qreal ImageItem::minHeight() const{
    if (!m_picture.isNull() && autoSize())
    {
        return m_picture.height();
    } else {
        return 0;
    }
}

void ImageItem::setVariable(const QString& content)
{
    if (m_variable!=content){
        QString oldValue = m_variable;
        m_variable=content;
        update();
        notify("variable", oldValue, m_variable);
    }
}

bool ImageItem::center() const
{
    return m_center;
}

void ImageItem::setCenter(bool center)
{
    if (m_center != center){
        m_center = center;
        update();
        notify("center",!center,center);
    }
}

bool ImageItem::keepAspectRatio() const
{
    return m_keepAspectRatio;
}

void ImageItem::setKeepAspectRatio(bool keepAspectRatio)
{
    if (m_keepAspectRatio != keepAspectRatio){
        m_keepAspectRatio = keepAspectRatio;
        update();
        notify("keepAspectRatio",!keepAspectRatio,keepAspectRatio);
    }
}

bool ImageItem::scale() const
{
    return m_scale;
}

void ImageItem::setScale(bool scale)
{
    if (m_scale != scale){
        m_scale = scale;
        update();
        notify("scale",!scale,scale);
    }
}
bool ImageItem::autoSize() const
{
    return m_autoSize;
}

void ImageItem::setAutoSize(bool autoSize)
{
    if (m_autoSize != autoSize){
        m_autoSize = autoSize;
        if (m_autoSize && !m_picture.isNull()){
            setWidth(drawImage().width());
            setHeight(drawImage().height());
            setPossibleResizeDirectionFlags(Fixed);
        } else {
            setPossibleResizeDirectionFlags(AllDirections);
        }
        update();
        notify("autoSize",!autoSize,autoSize);
    }
}

QString ImageItem::field() const
{
    return m_field;
}

void ImageItem::setField(const QString &field)
{
    if (m_field != field){
        QString oldValue = m_field;
        m_field = field;
        update();
        notify("field",oldValue,field);
    }
}

QString ImageItem::datasource() const
{
    return m_datasource;
}

void ImageItem::setDatasource(const QString &datasource)
{
    if (m_datasource != datasource){
        QString oldValue = m_datasource;
        m_datasource = datasource;
        update();
        notify("datasource",oldValue,datasource);
    }
}


void ImageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    if (isSelected()) painter->setOpacity(Const::SELECTION_OPACITY);
    else painter->setOpacity(qreal(opacity())/100);

    QPointF point = rect().topLeft();
    QImage img;

    if (m_scale && !drawImage().isNull()){
        img = drawImage().scaled(rect().width(), rect().height(), keepAspectRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    } else {
        img = drawImage();
    }

    qreal shiftHeight = rect().height() - img.height();
    qreal shiftWidth = rect().width() - img.width();

    if (m_center){
        if (shiftHeight<0 || shiftWidth<0){
            qreal cutX = 0;
            qreal cutY = 0;
            qreal cutWidth = img.width();
            qreal cutHeigth = img.height();

            if (shiftWidth > 0){
                point.setX(point.x()+shiftWidth/2);
            } else {
                cutX = fabs(shiftWidth/2);
                cutWidth += shiftWidth;
            }

            if (shiftHeight > 0){
                point.setY(point.y()+shiftHeight/2);
            } else {
                cutY = fabs(shiftHeight/2);
                cutHeigth += shiftHeight;
            }

            img = img.copy(cutX,cutY,cutWidth,cutHeigth);
        } else {
            point.setX(point.x()+shiftWidth/2);
            point.setY(point.y()+shiftHeight/2);
        }
    }

    if (img.isNull() && itemMode() == DesignMode){
        QString text;
        painter->setFont(transformToSceneFont(QFont("Arial",10)));
        painter->setPen(Qt::black);
        if (!datasource().isEmpty() && !field().isEmpty())
            text = datasource()+"."+field();
        else  if (m_useExternalPainter) text = tr("Ext."); else text = tr("Image");
        painter->drawText(rect().adjusted(4,4,-4,-4), Qt::AlignCenter, text );
    } else {
        if (m_externalPainter && m_useExternalPainter)
            m_externalPainter->paintByExternalPainter(this->patternName(), painter, option);
        else
            painter->drawImage(point,img);
    }

    ItemDesignIntf::paint(painter,option,widget);
    painter->restore();
}

void ImageItem::setImage(QImage value)
{
    if (m_picture != value){
        QImage oldValue = m_picture;
        m_picture = value;
        if (m_autoSize){
            setWidth(m_picture.width());
            setHeight(m_picture.height());
        }
        update();
        notify("image",oldValue,value);
    }
}

QImage ImageItem::image() const{
    return m_picture;
}

void ImageItem::setResourcePath(const QString &value){
    if (m_resourcePath != value){
        QString oldValue = m_resourcePath;
        m_resourcePath = value;
        update();
        notify("resourcePath", oldValue, value);
    }
}

ImageItem::Format ImageItem::format() const
{
    return m_format;
}

void ImageItem::setFormat(Format format)
{
    if (m_format!=format){
        Format oldValue = m_format;
        m_format=format;
        update();
        notify("format",oldValue,format);
    }
}

}

bool LimeReport::ImageItem::isEmpty() const
{
  return drawImage().isNull();
}
