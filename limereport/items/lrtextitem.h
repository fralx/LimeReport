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
#ifndef LRTEXTITEM_H
#define LRTEXTITEM_H
#include <QGraphicsTextItem>
#include <QtGui>
#include <QLabel>
#include "lritemdesignintf.h"
#include <qnamespace.h>

#include <QTextDocument>

namespace LimeReport {

class Tag;
class TextItem : public LimeReport::ContentItemDesignIntf {
    Q_OBJECT
    Q_ENUMS(AutoWidth)
    Q_ENUMS(AngleType)
    Q_PROPERTY(QString content READ content WRITE setContent)
    Q_PROPERTY(int margin READ marginSize WRITE setMarginSize)
    Q_PROPERTY(Qt::Alignment alignment READ alignment() WRITE setAlignment)
    Q_PROPERTY(AutoWidth autoWidth READ autoWidth() WRITE setAutoWidth)
    Q_PROPERTY(bool autoHeight READ autoHeight() WRITE setAutoHeight)
    Q_PROPERTY(QFont font READ font() WRITE setTextItemFont)
    Q_PROPERTY(int backgroundOpacity READ opacity WRITE setBackgroundOpacity)
    Q_PROPERTY(BGMode backgroundMode READ backgroundMode WRITE setBackgroundModeProperty)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColorProperty)
    Q_PROPERTY(QColor fontColor READ fontColor WRITE setFontColorProperty)
    Q_PROPERTY(AngleType angle READ angle WRITE setAngle)
    Q_PROPERTY(int foregroundOpacity READ foregroundOpacity WRITE setForegroundOpacity)
    Q_PROPERTY(bool trimValue READ trimValue WRITE setTrimValue)
    Q_PROPERTY(bool allowHTML READ allowHTML WRITE setAllowHTML)
    Q_PROPERTY(bool allowHTMLInFields READ allowHTMLInFields WRITE setAllowHTMLInFields)
public:

    enum AutoWidth{NoneAutoWidth,MaxWordLength,MaxStringLength};
    enum AngleType{Angle0,Angle90,Angle180,Angle270,Angle45,Angle315};

    void Init();
    TextItem(QObject* owner=0, QGraphicsItem* parent=0);
    ~TextItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    QString content() const;
    void setContent(const QString& value);

    //void setMarginSize(int value);


    void setAlignment(Qt::Alignment value);
    Qt::Alignment alignment(){return m_alignment;}

    void geometryChangedEvent(QRectF, QRectF);
    bool isNeedUpdateSize(RenderPass) const;
    void updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight);
    void expandContent(DataSourceManager *dataManager, RenderPass pass);

    void setAutoHeight(bool value);
    bool autoHeight() const {return m_autoHeight;}

    void setAutoWidth(AutoWidth value);
    AutoWidth autoWidth() const {return m_autoWidth;}

    bool canBeSplitted(int height) const;
    bool isSplittable() const { return true;}
    bool isEmpty() const{return m_text->isEmpty();}
    BaseDesignIntf* cloneUpperPart(int height, QObject *owner, QGraphicsItem *parent);
    BaseDesignIntf* cloneBottomPart(int height, QObject *owner, QGraphicsItem *parent);
    BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
    BaseDesignIntf* cloneEmpty(int height, QObject *owner, QGraphicsItem *parent);
    void objectLoadFinished();

    void setTextItemFont(QFont value);
    QWidget* defaultEditor();
    void setBackgroundOpacity(int value);
    void setBackgroundModeProperty(BGMode value);
    void setBackgroundColorProperty(QColor value);
    void setFontColorProperty(QColor value);
    AngleType angle() const;
    void setAngle(const AngleType& value);
    int foregroundOpacity(){return m_foregroundOpacity;}
    void setForegroundOpacity(int value);

    bool trimValue() const;
    void setTrimValue(bool trimValue);

    bool allowHTML() const;
    void setAllowHTML(bool allowHTML);

    bool allowHTMLInFields() const;
    void setAllowHTMLInFields(bool allowHTMLInFields);

protected:
    void updateLayout();
    bool isNeedExpandContent() const;
    QString replaceBR(QString text);
    QString replaceReturns(QString text);
    int fakeMarginSize();
private:
    void initText();
private:
    QString m_strText;

    //QTextLayout m_layout;
    QTextDocument* m_text;
    Qt::Alignment m_alignment;
    bool m_autoHeight;
    AutoWidth m_autoWidth;
    QSizeF m_textSize;
    AngleType m_angle;
    int m_foregroundOpacity;
    bool m_trimValue;
    bool m_allowHTML;
    bool m_allowHTMLInFields;
};

}
#endif // LRTEXTITEM_H
