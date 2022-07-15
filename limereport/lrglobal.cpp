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
#include <QString>
#include <QDebug>
#include "lrglobal.h"

namespace LimeReport {

QString extractClassName(QString className)
{
    int startPos=className.lastIndexOf("::");
    if(startPos==-1) startPos=0;
    else startPos+=2;
    return className.right(className.length()-startPos);
}

bool ReportSettings::suppressAbsentFieldsAndVarsWarnings() const
{
    return m_suppressAbsentFieldsAndVarsWarnings;
}

void ReportSettings::setSuppressAbsentFieldsAndVarsWarnings(bool suppressAbsentFieldsAndVarsWarnings)
{
    m_suppressAbsentFieldsAndVarsWarnings = suppressAbsentFieldsAndVarsWarnings;
}

QString escapeSimbols(const QString &value)
{
    QString result = value;
    result.replace("\"","\\\"");
    result.replace('\n',"\\n");
    return result;
}

QString replaceHTMLSymbols(const QString &value)
{
    QString result = value;
    result.replace("<","&lt;");
    result.replace(">","&gt;");
    return result;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
QVector<QString> normalizeCaptures(const QRegularExpressionMatch& reg){
#else
QVector<QString> normalizeCaptures(const QRegExp& reg){
#endif
    QVector<QString> result;
    foreach (QString cap, reg.capturedTexts()) {
        if (!cap.isEmpty())
            result.append(cap);
    }
    return result;
}

bool isColorDark(QColor color){
    qreal darkness = 1-(0.299*color.red() + 0.587*color.green() + 0.114*color.blue())/255;
    if(darkness<0.5){
        return false;
    } else {
        return true;
    }
}

ReportError::ReportError(const QString& message):std::runtime_error(message.toStdString()){}
IExternalPainter::~IExternalPainter(){}
IPainterProxy::~IPainterProxy(){}


#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 1)
QRegularExpression getRegEx(QString expression){
    return QRegularExpression(expression, QRegularExpression::DotMatchesEverythingOption);
}
QRegularExpression getVariableRegEx(){
    return QRegularExpression(
                Const::VARIABLE_RX,
                QRegularExpression::DotMatchesEverythingOption |
                QRegularExpression::CaseInsensitiveOption
           );
}
QRegularExpression getFieldRegEx(){
    return QRegularExpression(
                Const::FIELD_RX,
                QRegularExpression::DotMatchesEverythingOption |
                QRegularExpression::CaseInsensitiveOption
           );
}
QRegularExpression getScriptRegEx(){
    return QRegularExpression(
                Const::SCRIPT_RX,
                QRegularExpression::DotMatchesEverythingOption |
                QRegularExpression::CaseInsensitiveOption
           );
}
QRegularExpression getGroupFunctionRegEx(QString functionName){
    return QRegularExpression(
                QString(Const::GROUP_FUNCTION_RX).arg(functionName),
                QRegularExpression::DotMatchesEverythingOption |
                QRegularExpression::InvertedGreedinessOption
           );
}
QRegularExpression getGroupFunctionNameRegEx(QString functionName){
    return QRegularExpression(
                QString(Const::GROUP_FUNCTION_NAME_RX).arg(functionName),
                QRegularExpression::DotMatchesEverythingOption |
                QRegularExpression::InvertedGreedinessOption
           );
}
QRegularExpression getNamedVariableRegEx(QString variableName){
    return QRegularExpression(
                QString(Const::NAMED_VARIABLE_RX).arg(variableName),
                QRegularExpression::DotMatchesEverythingOption
           );
}
#endif


} //namespace LimeReport
