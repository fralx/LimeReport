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
#ifndef LRGLOBAL_H
#define LRGLOBAL_H
#include "qglobal.h"
#include <stdexcept>
#include <QString>
#include <QStyleOptionViewItem>
#include <QtGlobal>

#if defined(LIMEREPORT_EXPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_EXPORT
#elif defined (LIMEREPORT_IMPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_IMPORT
#else
#  define LIMEREPORT_EXPORT   /**/
#endif

namespace LimeReport {

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#if QT_VERSION >= 0x050800
Q_NAMESPACE
#endif

namespace Const{
    int const DEFAULT_GRID_STEP = 1;
    int const RESIZE_HANDLE_SIZE = 5;
    int const SELECTION_PEN_SIZE = 1;
    int const MINIMUM_ITEM_WIDTH = 2*RESIZE_HANDLE_SIZE;
    int const MINIMUM_ITEM_HEIGHT = 2*RESIZE_HANDLE_SIZE;
    double const RESIZE_ZONE_OPACITY = 0.5;
    double const SELECTED_RESIZE_ZONE_OPACITY = 0.6;
    Qt::GlobalColor const RESIZE_ZONE_COLOR = Qt::green;
    Qt::GlobalColor const SELECTION_COLOR = Qt::red;
    Qt::GlobalColor const JOIN_COLOR = Qt::blue;
    double const SELECTION_COLOR_OPACITY = 0.6;
    const qreal fontFACTOR = 3.5;
    const int mmFACTOR = 10;
    const int itemPaleteIconSize = 24;
    const qreal minSpaceBorder = 10;
    const QString bandTAG = "band";
    const Qt::GlobalColor BAND_NAME_LABEL_COLOR = Qt::yellow;
    const Qt::GlobalColor BAND_NAME_BORDER_COLOR = Qt::darkYellow;
    const qreal BAND_MARKER_OPACITY = 1;
    const qreal LAYOUT_MARKER_OPACITY = 0.3;
    const qreal BAND_NAME_AREA_OPACITY = 0.3;
    const qreal BAND_NAME_TEXT_OPACITY = 0.6;
    const qreal SELECTION_OPACITY = 0.3;
    const QString FIELD_RX = "\\$D\\s*\\{\\s*([^{}]*)\\s*\\}";
    const QString VARIABLE_RX = "\\$V\\s*\\{\\s*(?:([^\\{\\},]*)|(?:([^\\{\\}]*)\\s*,\\s*([^\\{\\}]*)))\\s*\\}";
    const QString NAMED_VARIABLE_RX = "\\$V\\s*\\{\\s*(?:(%1)|(?:(%1)\\s*,\\s*([^\\{\\}]*)))\\s*\\}";
    const QString SCRIPT_RX = "\\$S\\s*\\{(.*)\\}";    
    const QString GROUP_FUNCTION_PARAM_RX = "\\(\\s*((?:(?:\\\")|(?:))(?:(?:\\$(?:(?:D\\{\\s*\\w*..*\\})|(?:V\\{\\s*\\w*\\s*\\})|(?:S\\{.+\\})))|(?:\\w*))(?:(?:\\\")|(?:)))(?:(?:\\s*,\\s*(?:\\\"(\\w*)\\\"))|(?:))(?:(?:\\s*,\\s*(?:(\\w*)))|(?:))\\)";
    const int DATASOURCE_INDEX = 3;
    const int VALUE_INDEX = 2;
    const int EXPRESSION_ARGUMENT_INDEX = 1;

    const QString GROUP_FUNCTION_RX = "(%1\\s*"+GROUP_FUNCTION_PARAM_RX+")";
    const QString GROUP_FUNCTION_NAME_RX = "%1\\s*\\((.*[^\\)])\\)";
    const int SCENE_MARGIN = 50;
    const QString FUNCTION_MANAGER_NAME = "LimeReport";
    const QString DATAFUNCTIONS_MANAGER_NAME = "DatasourceFunctions";
    const QString EOW("~!@#$%^&*()+{}|:\"<>?,/;'[]\\-=");
    const int DEFAULT_TAB_INDENTION = 4;
    const int DOCKWIDGET_MARGINS = 4;

    const char SCRIPT_SIGN = 'S';
    const char FIELD_SIGN = 'D';
    const char VARIABLE_SIGN = 'V';
}
    QString extractClassName(QString className);
    QString escapeSimbols(const QString& value);
    QString replaceHTMLSymbols(const QString &value);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    QVector<QString> normalizeCaptures(const QRegularExpressionMatch &reg);
#else
    QVector<QString> normalizeCaptures(const QRegExp &reg);    
#endif
    bool isColorDark(QColor color);

    enum ExpandType {EscapeSymbols, NoEscapeSymbols, ReplaceHTMLSymbols};
    enum RenderPass {FirstPass = 1, SecondPass = 2};
    enum ArrangeType {AsNeeded, Force};
    enum ScaleType {FitWidth, FitPage, OneToOne, Percents};
    enum PreviewHint{ShowAllPreviewBars = 0,
                     HidePreviewToolBar = 1,
                     HidePreviewMenuBar = 2,
                     HidePreviewStatusBar = 4,
                     HideAllPreviewBar = 7,
                     PreviewBarsUserSetting = 8};

    Q_DECLARE_FLAGS(PreviewHints, PreviewHint)
    Q_FLAGS(PreviewHints)

    class LIMEREPORT_EXPORT ReportError : public std::runtime_error{
    public:
        ReportError(const QString& message);
    };

    class LIMEREPORT_EXPORT ReportSettings{
    public:
        ReportSettings():m_suppressAbsentFieldsAndVarsWarnings(false){}
        void setDefaultValues(){m_suppressAbsentFieldsAndVarsWarnings = false;}
        bool suppressAbsentFieldsAndVarsWarnings() const;
        void setSuppressAbsentFieldsAndVarsWarnings(bool suppressAbsentFieldsAndVarsWarnings);
    private:
        bool m_suppressAbsentFieldsAndVarsWarnings;
    };

    class LIMEREPORT_EXPORT IExternalPainter{
    public:
        virtual void paintByExternalPainter(const QString& objectName, QPainter* painter, const QStyleOptionGraphicsItem* options) = 0;
        virtual ~IExternalPainter();
    };

    class LIMEREPORT_EXPORT IPainterProxy{
    public:
        virtual void setExternalPainter(IExternalPainter* externalPainter) = 0;
        virtual ~IPainterProxy();
    };

#if QT_VERSION < 0x050000
    typedef QStyleOptionViewItemV4 StyleOptionViewItem;
#else
    typedef QStyleOptionViewItem StyleOptionViewItem;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
    QRegularExpression getRegEx(QString expression);
    QRegularExpression getVariableRegEx();
    QRegularExpression getFieldRegEx();
    QRegularExpression getScriptRegEx();
    QRegularExpression getGroupFunctionRegEx(QString functionName);
    QRegularExpression getGroupFunctionNameRegEx(QString functionName);
    QRegularExpression getNamedVariableRegEx(QString variableName);
#endif


    class LIMEREPORT_EXPORT Enums
    {
    public:
        enum VariableDataType {Undefined, String, Bool, Int, Real, Date, Time, DateTime};
#if QT_VERSION >= 0x050500
        Q_ENUM(VariableDataType)
#else
        Q_ENUMS(VariableDataType)
#endif
    private:
        Enums(){}
        Q_GADGET
    };

    typedef Enums::VariableDataType VariableDataType;

} // namespace LimeReport

Q_DECLARE_OPERATORS_FOR_FLAGS(LimeReport::PreviewHints)

#endif // GLOBAL_H
