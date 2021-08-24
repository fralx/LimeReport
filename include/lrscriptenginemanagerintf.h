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
#ifndef LRSCRIPTENGINEMANAGERINTF_H
#define LRSCRIPTENGINEMANAGERINTF_H
#include "qglobal.h"

#if QT_VERSION >= 0x050600
    #ifndef USE_QTSCRIPTENGINE
        #ifndef USE_QJSENGINE
            #define USE_QJSENGINE
        #endif
    #endif
#else
    #ifndef USE_QTSCRIPTENGINE
        #define USE_QTSCRIPTENGINE
    #endif
#endif

#ifdef USE_QJSENGINE
#include <QQmlEngine>
#else
#include <QtScript/QScriptEngine>
#endif

namespace LimeReport{

#ifdef USE_QJSENGINE
    typedef QJSEngine ScriptEngineType;
    typedef QJSValue ScriptValueType;
    template <typename T>
    static inline QJSValue getJSValue(QJSEngine &e, T *p)
    {
        QJSValue res = e.newQObject(p);
        QQmlEngine::setObjectOwnership(p, QQmlEngine::CppOwnership);
        return res;
    }
#else
    typedef QScriptEngine ScriptEngineType;
    typedef QScriptValue ScriptValueType;
#endif

class IScriptEngineManager{
public:
    virtual ScriptEngineType* scriptEngine() = 0;
#ifdef USE_QTSCRIPTENGINE
    virtual bool addFunction(const QString& name, ScriptEngineType::FunctionSignature function,
                             const QString& category="", const QString& description="") = 0;
#endif
    virtual bool addFunction(const QString &name, const QString& script,
                             const QString &category="", const QString &description="") = 0;
    virtual const QString& lastError() const = 0;
    virtual ScriptValueType moveQObjectToScript(QObject* object, const QString objectName) = 0;
    virtual ~IScriptEngineManager(){}

};

} //namespace LimeReport

#endif // LRSCRIPTENGINEMANAGERINTF_H
