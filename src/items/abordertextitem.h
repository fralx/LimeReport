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
#ifndef ABORDERTEXTITEM_H
#define ABORDERTEXTITEM_H
#include <QGraphicsTextItem>
#include <QtGui>
#include <QLabel>
#include "aitemdesignintf.h"
#include <qnamespace.h>

#include <QTextDocument>

namespace ArReport {

class Tag;
class TextItem : public ArReport::ItemDesignIntf {
    Q_OBJECT
    Q_ENUMS(AutoWidth)
    Q_PROPERTY(QString content READ text WRITE setText)
    Q_PROPERTY(int margin READ spaceBorderSize WRITE setSpaceBorderSize)
    Q_PROPERTY(Qt::Alignment alignment READ alignment() WRITE setAlignment)
    Q_PROPERTY(AutoWidth autoWidth READ autoWidth() WRITE setAutoWidth())
    Q_PROPERTY(bool autoHeight READ autoHeight() WRITE setAutoHeight())
    Q_PROPERTY(QFont font READ font() WRITE setTextItemFont())
public:

    enum AutoWidth{NoneAutoWidth,MaxWordLength,MaxStringLength};

    void Init();
    TextItem(QObject* owner,QGraphicsItem* parent);
    TextItem(const QString& text,QObject* owner = 0, QGraphicsItem* parent = 0);
    ~TextItem();

    void paint(QPainter* ppainter, const QStyleOptionGraphicsItem*, QWidget*);
    QString text() const;
    void setText(QString value);

    void setSpaceBorderSize(int value);
    int spaceBorderSize() const {return m_spaceBorder;}

    void setAlignment(Qt::Alignment value);
    Qt::Alignment alignment(){return m_alignment;}

    virtual void setGeometry(QRectF rect);
    virtual bool isNeedUpdateSize() const;
    virtual void updateItemSize(RenderPass pass, int);
    void expandContent(RenderPass pass);

    void setAutoHeight(bool value){m_autoHeight=value;}
    bool autoHeight() const {return m_autoHeight;}

    void setAutoWidth(AutoWidth value){m_autoWidth=value;}
    AutoWidth autoWidth() const {return m_autoWidth;}

    virtual bool canBeSliced(int height) const;
    virtual bool isEmpty() const{return m_text->isEmpty();}
    virtual BaseDesignIntf* cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent);
    virtual BaseDesignIntf* cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent);
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
    virtual BaseDesignIntf* cloneEmpty(int height, QObject *owner, QGraphicsItem *parent);
    virtual void objectLoadFinished();

    void setTextItemFont(QFont value);
protected:
    void updateLayout();
    bool isNeedExpandContent() const;
    QString expandDataFields(QString context);
private:
    void initText();
private:
    QString m_strText;
    int     m_spaceBorder;
    QTextLayout m_layout;
    QTextDocument* m_text;
    Qt::Alignment m_alignment;
    bool m_autoHeight;
    AutoWidth m_autoWidth;
    QSizeF m_textSize;
    QFont m_font;
};

}
#endif // ABORDERTEXTITEM_H
