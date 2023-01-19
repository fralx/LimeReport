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
#ifndef LRIMAGEITEM_H
#define LRIMAGEITEM_H
#include "lritemdesignintf.h"
#include "lreditableimageitemintf.h"
#include <QtGlobal>

namespace LimeReport{

class ImageItem : public ItemDesignIntf, public IPainterProxy, public IEditableImageItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage)
    Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QString datasource READ datasource WRITE setDatasource)
    Q_PROPERTY(QString field READ field WRITE setField)
    Q_PROPERTY(Format format READ format WRITE setFormat)
    Q_PROPERTY(bool autoSize READ autoSize WRITE setAutoSize)
    Q_PROPERTY(bool scale READ scale WRITE setScale)
    Q_PROPERTY(bool keepAspectRatio READ keepAspectRatio WRITE setKeepAspectRatio)
    Q_PROPERTY(bool center READ center WRITE setCenter)
    Q_PROPERTY(QString resourcePath READ resourcePath WRITE setResourcePath)
    Q_PROPERTY(QString variable READ variable WRITE setVariable)
    Q_PROPERTY(bool watermark READ isWatermark WRITE setWatermark)
    Q_PROPERTY(bool useExternalPainter READ useExternalPainter WRITE setUseExternalPainter)

public:
    enum Format {
        Binary  = 0,
        Hex     = 1,
        Base64  = 2
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(Format)
#else
    Q_ENUMS(Format)
#endif

    ImageItem(QObject *owner, QGraphicsItem *parent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setImage(QImage value);
    QImage image() const;
    void setResourcePath(const QString &value);
    QString resourcePath() const;
    QString datasource() const;
    void setDatasource(const QString &datasource);
    QString field() const;
    void setField(const QString &field);

    bool autoSize() const;
    void setAutoSize(bool autoSize);
    bool scale() const;
    void setScale(bool scale);
    bool keepAspectRatio() const;
    void setKeepAspectRatio(bool keepAspectRatio);
    bool center() const;
    void setCenter(bool center);
    Format format() const;
    void setFormat(Format format);
    qreal minHeight() const;

    QString variable(){ return m_variable;}
    void setVariable(const QString& variable);

    void setExternalPainter(IExternalPainter* externalPainter){ m_externalPainter = externalPainter;}

    bool useExternalPainter() const;
    void setUseExternalPainter(bool value);

    QWidget* defaultEditor();

    QByteArray imageAsByteArray() const;
    void setImageAsByteArray(QByteArray image);
    QString fileFilter() const;
protected:
    BaseDesignIntf* createSameTypeItem(QObject *owner, QGraphicsItem *parent);
    void updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight);
    bool isNeedUpdateSize(RenderPass) const;
    bool drawDesignBorders() const {return m_picture.isNull();}
    void loadPictureFromVariant(QVariant& data);
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
    QImage drawImage() const;
private:
    QImage  m_picture;
    bool m_useExternalPainter;
    IExternalPainter* m_externalPainter;
    QString m_resourcePath;
    QString m_datasource;
    QString m_field;
    bool    m_autoSize;
    bool    m_scale;
    bool    m_keepAspectRatio;
    bool    m_center;
    Format  m_format;
    QString m_variable;


    // BaseDesignIntf interface
  public:
    bool isEmpty() const override;
};

}
#endif // LRIMAGEITEM_H
