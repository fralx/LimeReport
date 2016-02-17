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
#ifndef GLOBAL_H
#define GLOBAL_H
#include "qglobal.h"
#include <stdexcept>
#include <QString>

#if defined(LIMEREPORT_EXPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_EXPORT
#elif defined (LIMEREPORT_IMPORTS)
#  define LIMEREPORT_EXPORT Q_DECL_IMPORT
#else
#  define LIMEREPORT_EXPORT   /**/
#endif

namespace LimeReport {
    const qreal fontFACTOR = 3.5;
    const int mmFACTOR = 10;
    const int itemPaleteIconSize = 24;
    const qreal minSpaceBorder = 10;
    QString extractClassName(QString className);
    const QString bandTAG = "band";
    const qreal BAND_MARKER_OPACITY = 1;
    const qreal LAYOUT_MARKER_OPACITY = 0.3;
    const qreal BAND_NAME_AREA_OPACITY = 0.3;
    const qreal BAND_NAME_TEXT_OPACITY = 0.6;
    const qreal SELECTION_OPACITY = 0.3;
    enum RenderPass {FirstPass, SecondPass};
    enum ArrangeType {AsNeeded, Force};
    const QString FIELD_RX = "\\$D\\s*\\{\\s*([^\\s{}]*)\\s*\\}";
    const QString VARIABLE_RX = "\\$V\\s*\\{\\s*([^\\s{}]*)\\s*\\}";
    const QString SCRIPT_RX = "\\$S\\s*\\{(.*)\\}";
    const QString GROUP_FUNCTION_PARAM_RX = "\\(\\s*(((?:\\\"?\\$D\\s*\\{\\s*)|(?:\\\"?\\$V\\s*\\{\\s*)|(?:\\\"))(\\w+\\.?\\w+)((?:\\\")|(?:\\s*\\}\\\"?\\s*)))\\s*,\\s*\\\"(\\w+)\\\"\\s*\\)";
    const int DATASOURCE_INDEX = 6;
    const int VALUE_INDEX = 2;
    const QString GROUP_FUNCTION_RX = "(%1\\s*"+GROUP_FUNCTION_PARAM_RX+")";
    const QString GROUP_FUNCTION_NAME_RX = "%1\\s*\\((.*[^\\)])\\)";
    const int SCENE_MARGIN = 50;
    class ReportError : public std::runtime_error{
    public:
        ReportError(const QString& message):std::runtime_error(message.toStdString()){}
    };

} // namespace LimeReport



#endif // GLOBAL_H
