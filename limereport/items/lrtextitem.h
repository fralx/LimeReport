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
#ifndef LRTEXTITEM_H
#define LRTEXTITEM_H
#include <QGraphicsTextItem>
#include <QtGui>
#include <QLabel>
#include <QTextDocument>
#include <QtGlobal>

#include "lritemdesignintf.h"
#include "lritemdesignintf.h"
#include "lrpageinitintf.h"

namespace LimeReport {

class Tag;
class TextItem : public ContentItemDesignIntf, IPageInit {
    Q_OBJECT
    Q_PROPERTY(QString content READ content WRITE setContent)
    Q_PROPERTY(int margin READ marginSize WRITE setMarginSize)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)
    Q_PROPERTY(AutoWidth autoWidth READ autoWidth WRITE setAutoWidth)
    Q_PROPERTY(bool autoHeight READ autoHeight WRITE setAutoHeight)
    Q_PROPERTY(QFont font READ font WRITE setTextItemFont)
    Q_PROPERTY(int backgroundOpacity READ opacity WRITE setBackgroundOpacity)
    Q_PROPERTY(BGMode backgroundMode READ backgroundMode WRITE setBackgroundModeProperty)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColorProperty)
    Q_PROPERTY(QColor fontColor READ fontColor WRITE setFontColorProperty)
    Q_PROPERTY(AngleType angle READ angle WRITE setAngle)
    Q_PROPERTY(int foregroundOpacity READ foregroundOpacity WRITE setForegroundOpacity)
    Q_PROPERTY(bool underlines READ underlines WRITE setUnderlines)
    Q_PROPERTY(bool adaptFontToSize READ adaptFontToSize WRITE setAdaptFontToSize)
    Q_PROPERTY(bool trimValue READ trimValue WRITE setTrimValue)
    Q_PROPERTY(int lineSpacing READ lineSpacing WRITE setLineSpacing)
    Q_PROPERTY(int underlineLineSize READ underlineLineSize WRITE setUnderlineLineSize)
    Q_PROPERTY(bool allowHTML READ allowHTML WRITE setAllowHTML)
    Q_PROPERTY(bool allowHTMLInFields READ allowHTMLInFields WRITE setAllowHTMLInFields)
    Q_PROPERTY(QString format READ format WRITE setFormat)
    Q_PROPERTY(ValueType valueType READ valueType WRITE setValueType)
    Q_PROPERTY(QString followTo READ followTo WRITE setFollowTo)
    Q_PROPERTY(BrushStyle backgroundBrushStyle READ backgroundBrushStyle WRITE setBackgroundBrushStyle)
    Q_PROPERTY(qreal textIndent READ textIndent WRITE setTextIndent)
    Q_PROPERTY(Qt::LayoutDirection textLayoutDirection READ textLayoutDirection WRITE setTextLayoutDirection)
    Q_PROPERTY(bool fillInSecondPass READ fillInSecondPass WRITE setFillInSecondPass)
    Q_PROPERTY(bool watermark READ isWatermark WRITE setWatermark)
    Q_PROPERTY(bool replaceCRwithBR READ isReplaceCarriageReturns WRITE setReplaceCarriageReturns)
    Q_PROPERTY(bool hideIfEmpty READ hideIfEmpty WRITE setHideIfEmpty)
    Q_PROPERTY(int fontLetterSpacing READ fontLetterSpacing WRITE setFontLetterSpacing)
public:

    enum AutoWidth{NoneAutoWidth, MaxWordLength, MaxStringLength};
    enum AngleType{Angle0, Angle90, Angle180, Angle270, Angle45, Angle315};
    enum ValueType{Default, DateTime, Double};
#if QT_VERSION >= 0x050500
    Q_ENUM(AutoWidth)
    Q_ENUM(AngleType)
    Q_ENUM(ValueType)
#else
    Q_ENUMS(AutoWidth)
    Q_ENUMS(AngleType)
    Q_ENUMS(ValueType)
#endif

    void Init();
    TextItem(QObject* owner=0, QGraphicsItem* parent=0);
    ~TextItem();

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*);
    QString content() const;
    void setContent(const QString& value);

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

    void setAdaptFontToSize(bool value);
    bool adaptFontToSize() const {return m_adaptFontToSize;}

    bool canBeSplitted(int height) const;
    bool isSplittable() const { return true;}
    bool isEmpty() const{return m_strText.trimmed().isEmpty();}
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
    bool underlines(){return m_underlines;}
    void setUnderlines(bool value);

    bool trimValue() const;
    void setTrimValue(bool trimValue);

    int lineSpacing() const;
    void setLineSpacing(int value);

    int underlineLineSize() const;
    void setUnderlineLineSize(int value);

    bool allowHTML() const;
    void setAllowHTML(bool allowHTML);

    bool allowHTMLInFields() const;
    void setAllowHTMLInFields(bool allowHTMLInFields);

    QString format() const;
    void setFormat(const QString &format);

    ValueType valueType() const;
    void setValueType(const ValueType valueType);

    QSizeF textSize(){ return m_textSize;}
    QString followTo() const;
    void setFollowTo(const QString &followTo);
    void setFollower(TextItem* follower);
    void clearFollower();
    bool hasFollower() const;
    TextItem* follower() const { return m_follower;}
    bool initFollower(QString follower);

    // IPageInit interface
    void pageObjectHasBeenLoaded();

    typedef QSharedPointer<QTextDocument> TextPtr;

    qreal textIndent() const;
    void setTextIndent(const qreal &textIndent);
    Qt::LayoutDirection textLayoutDirection() const;
    void setTextLayoutDirection(const Qt::LayoutDirection &textLayoutDirection);

    void setWatermark(bool watermark);
    
    bool isReplaceCarriageReturns() const;
    void setReplaceCarriageReturns(bool isReplaceCarriageReturns);

    bool hideIfEmpty() const;
    void setHideIfEmpty(bool hideIfEmpty);

    int fontLetterSpacing() const;
    void setFontLetterSpacing(int fontLetterSpacing);

protected:
    void updateLayout();
    bool isNeedExpandContent() const;
    QString replaceBR(QString text) const;
    QString replaceReturns(QString text) const;
    int fakeMarginSize() const;
    QString getTextPart(int height, int skipHeight);
    void restoreLinksEvent();
    void preparePopUpMenu(QMenu &menu);
    void processPopUpAction(QAction *action);
private:
    void initTextSizes() const;
    void setTextFont(TextPtr text, const QFont &value) const;
    void adaptFontSize(TextPtr text) const;
    QString formatDateTime(const QDateTime &value);
    QString formatNumber(const double value);
    QString formatFieldValue();
    QString extractText(QTextBlock& curBlock, int height);
    TextPtr textDocument() const;
private:
    QString m_strText;
    Qt::Alignment m_alignment;
    bool m_autoHeight;
    AutoWidth m_autoWidth;
    QSizeF mutable m_textSize;
    qreal  mutable m_firstLineSize;
    AngleType m_angle;
    int m_foregroundOpacity;
    bool m_underlines;
    bool m_adaptFontToSize;
    bool m_trimValue;
    int m_lineSpacing;
    int m_underlineLineSize;
    bool m_allowHTML;
    bool m_allowHTMLInFields;
    bool m_replaceCarriageReturns;

    QString m_format;
    ValueType m_valueType;
    QString   m_followTo;
    TextItem* m_follower;
    qreal m_textIndent;
    Qt::LayoutDirection m_textLayoutDirection;
    bool m_hideIfEmpty;
    int m_fontLetterSpacing;
};

}
#endif // LRTEXTITEM_H
