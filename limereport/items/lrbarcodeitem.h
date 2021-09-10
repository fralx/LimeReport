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
#ifndef LRBARCODEITEM_H
#define LRBARCODEITEM_H
#include "lritemdesignintf.h"
#include <QtGlobal>

namespace LimeReport{

class BarcodeItem : public LimeReport::ContentItemDesignIntf {
    Q_OBJECT
    Q_PROPERTY(QString content READ content WRITE setContent)
    Q_PROPERTY(BarcodeType barcodeType READ barcodeType WRITE setBarcodeType)
    Q_PROPERTY(QString datasource READ datasource WRITE setDatasource)
    Q_PROPERTY(QString field READ field WRITE setField)
    Q_PROPERTY(QString testValue READ designTestValue WRITE setDesignTestValue)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(int whitespace READ whitespace WRITE setWhitespace)
    Q_PROPERTY(AngleType angle READ angle WRITE setAngle)
    Q_PROPERTY(int barcodeWidth READ barcodeWidth WRITE setBarcodeWidth)
    Q_PROPERTY(int securityLevel READ securityLevel WRITE setSecurityLevel)
    Q_PROPERTY(int pdf417CodeWords READ pdf417CodeWords WRITE setPdf417CodeWords)
    Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode)
    Q_PROPERTY(bool hideText READ hideText WRITE setHideText)
    Q_PROPERTY(int option3 READ option3 WRITE setOption3)
    Q_PROPERTY(bool hideIfEmpty READ hideIfEmpty WRITE setHideIfEmpty)
public:
    enum BarcodeType {
        CODE11          =1,
        C25MATRIX       =2,
        C25INTER        =3,
        C25IATA         =4,
        C25LOGIC        =6,
        C25IND          =7,
        CODE39          =8,
        EXCODE39        =9,
        EANX            =13,
        EANX_CHK        =14,
        EAN128          =16,
        CODABAR         =18,
        CODE128         =20,
        DPLEIT          =21,
        DPIDENT         =22,
        CODE16K         =23,
        CODE93          =25,
        FLAT            =28,
        RSS14           =29,
        RSS_LTD         =30,
        RSS_EXP         =31,
        TELEPEN         =32,
        UPCA            =34,
        UPCE            =37,
        POSTNET         =40,
        MSI_PLESSEY     =47,
        FIM             =49,
        LOGMARS         =50,
        PHARMA          =51,
        PZN             =52,
        PHARMA_TWO      =53,
        PDF417          =55,
        PDF417TRUNC     =56,
        MAXICODE        =57,
        QRCODE          =58,
        CODE128B        =60,
        AUSPOST         =63,
        AUSREPLY        =66,
        AUSROUTE        =67,
        AUSREDIRECT     =68,
        ISBNX           =69,
        RM4SCC          =70,
        DATAMATRIX      =71,
        ITF14           =72,
        CODABLOCKF      =74,
        NVE18           =75,
        KOREAPOST       =77,
        RSS14STACK      =79,
        RSS14STACK_OMNI =80,
        RSS_EXPSTACK    =81,
        PLANET          =82,
        MICROPDF417     =84,
        ONECODE         =85,
        PLESSEY         =86,
        KIX             =90,
        AZTEC           =92,
        DAFT            =93,
        ITALYPOST       =94,
        DPD             =96,
        MICROQR         =97,
        HIBC_128        =98,
        HIBC_39         =99,
        HIBC_DM         =102,
        HIBC_QR         =104,
        HIBC_PDF        =106,
        HIBC_MICPDF     =108,
        HIBC_BLOCKF     =110,
        HIBC_AZTEC      =112,
        DOTCODE         =115,
        HANXIN          =116,
        TELEPEN_NUM     =128,
        CODE32          =129,
//        EANX_CC         =130,
//        EAN128_CC       =131,
//        RSS14_CC        =132,
//        RSS_LTD_CC      =133,
//        RSS_EXP_CC      =134,
//        UPCA_CC         =135,
//        UPCE_CC         =136,
//        RSS14STACK_CC   =137,
//        RSS14_OMNI_CC   =138,
//        RSS_EXPSTACK_CC =139,
        CHANNEL         =140,
        CODEONE         =141,
        GRIDMATRIX      =142,
        UPNQR           =143

    };

    enum AngleType{Angle0,Angle90,Angle180,Angle270};

    enum InputMode{
        DATA_INPUT_MODE     = 0,
        UNICODE_INPUT_MODE  = 1,
        GS1_INPUT_MODE      = 2,
        KANJI_INPUT_MODE    = 3,
        SJIS_INPUT_MODE     = 4
    };
#if QT_VERSION >= 0x050500
    Q_ENUM(BarcodeType)
    Q_ENUM(AngleType)
    Q_ENUM(InputMode)
#else
    Q_ENUMS(BarcodeType)
    Q_ENUMS(AngleType)
    Q_ENUMS(InputMode)
#endif
    BarcodeItem(QObject *owner, QGraphicsItem *parent);
    ~BarcodeItem();
    virtual BaseDesignIntf* createSameTypeItem(QObject *owner, QGraphicsItem *parent);
    virtual void paint(QPainter *ppainter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void updateItemSize(DataSourceManager *dataManager, RenderPass pass, int maxHeight);
    virtual bool isNeedUpdateSize(RenderPass pass) const;
    void setContent(const QString& content);
    QString content() const {return m_content;}
    void setBarcodeType(BarcodeType value);
    BarcodeType barcodeType(){return m_barcodeType;}
    QString datasource() const;
    void setDatasource(const QString &datasource);
    QString field() const;
    void setField(const QString &field);
    void setDesignTestValue(QString value);
    QString designTestValue(){return m_designTestValue;}
    QColor foregroundColor(){return m_foregroundColor;}
    void setForegroundColor(QColor value);
    QColor backgroundColor(){return m_backgroundColor;}
    void setBackgroundColor(QColor value);
    int whitespace(){return m_whitespace;}
    void setWhitespace(int value);
    AngleType angle() const;
    void setAngle(const AngleType &angle);
    int barcodeWidth() const;
    void setBarcodeWidth(int barcodeWidth);
    int securityLevel() const;
    void setSecurityLevel(int securityLevel);
    int pdf417CodeWords() const;
    void setPdf417CodeWords(int pdf417CodeWords);
    InputMode inputMode() const;
    void setInputMode(const InputMode &inputMode);
    bool hideText() const;
    void setHideText(bool hideText);
    int option3() const;
    void setOption3(int option3);
    bool hideIfEmpty() const;
    void setHideIfEmpty(bool hideIfEmpty);
    bool isEmpty() const;

private:
    void expandContent(QString data, DataSourceManager *dataManager, RenderPass pass);

private:
    QString m_content;
    QString m_datasource;
    QString m_field;
    QString m_designTestValue;
    BarcodeType m_barcodeType;
    QColor m_foregroundColor;
    QColor m_backgroundColor;
    int m_whitespace;
    AngleType m_angle;
    int m_barcodeWidth;
    int m_securityLevel;
    int m_pdf417CodeWords;
    InputMode m_inputMode;
    bool m_hideText;
    int m_option3;
    bool m_hideIfEmpty;
};

}
#endif // LRBARCODEITEM_H
