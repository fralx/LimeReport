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
#include "lritemdesignintf.h"
#include "lrbasedesignintf.h"
#include "lrpagedesignintf.h"
#include "lrdatasourcemanager.h"
#include "lrscriptenginemanager.h"
#include "lrglobal.h"

#include <QDebug>
#include <QRegExp>
#include <QVariant>

namespace LimeReport{

ItemDesignIntf::ItemDesignIntf(const QString &xmlTypeName, QObject* owner, QGraphicsItem* parent) :
    BaseDesignIntf(xmlTypeName,owner,parent), m_itemLocation(Band), m_stretchToMaxHeight(false){
    initFlags();
}

void ItemDesignIntf::setItemLocation(LocationType location)
{

    if (m_itemLocation!=location){
        LocationType oldValue = m_itemLocation;
        m_itemLocation=location;
        if(!isLoading()){
            if (location==Band){
                QGraphicsItem *parentBand=bandByPos();
                if (parentBand){
                    QPointF parentPos = parentBand->mapFromItem(parentItem(),x(),y());
                    setParentItem(parentBand);
                    setParent(dynamic_cast<BandDesignIntf*>(parentBand));
                    setPos(parentPos);
                } else {
                    m_itemLocation=Page;
                }
            } else {
                if (scene()){
                    PageItemDesignIntf* page = dynamic_cast<PageDesignIntf*>(scene())->pageItem();
                    QPointF parentPos = page->mapFromItem(parentItem(),x(),y());
                    setParentItem(page);
                    setParent(page);
                    setPos(parentPos);
                }
            }
            notify("locationType",oldValue,location);
        }
    }
}

void ItemDesignIntf::setStretchToMaxHeight(bool value)
{
    if (m_stretchToMaxHeight!=value){
        bool oldValue = m_stretchToMaxHeight;
        m_stretchToMaxHeight=value;
        notify("stretchToMaxHeight",oldValue,value);
    }
}

BaseDesignIntf *ItemDesignIntf::cloneEmpty(int height, QObject *owner, QGraphicsItem *parent)
{
    BaseDesignIntf* spacer = new Spacer(owner,parent);
    spacer->initFromItem(this);
    spacer->setHeight(height);
    return spacer;
}

QGraphicsItem * ItemDesignIntf::bandByPos()
{
    foreach(QGraphicsItem *item,collidingItems()){
        if (dynamic_cast<BandDesignIntf*>(item)){
            return item;
        }
    }
    return 0;
}

void ItemDesignIntf::initFlags()
{
    BaseDesignIntf::initFlags();
    if ((itemMode()&DesignMode) || (itemMode()&EditMode)){
        setFlag(QGraphicsItem::ItemIsMovable);
    } else {
        setFlag(QGraphicsItem::ItemIsMovable,false);
    }
}

QString ContentItemDesignIntf::expandDataFields(QString context, ExpandType expandType, DataSourceManager* dataManager)
{   
    QRegExp rx(Const::FIELD_RX);

    if (context.contains(rx)){
        while ((rx.indexIn(context))!=-1){
            QString field=rx.cap(1);

            if (dataManager->containsField(field)) {
                QString fieldValue;
                if (expandType == EscapeSymbols) {
                    if (dataManager->fieldData(field).isNull()) {
                        fieldValue="\"\"";
                    } else {
                        fieldValue = escapeSimbols(dataManager->fieldData(field).toString());
                        switch (dataManager->fieldData(field).type()) {
                        case QVariant::Char:
                        case QVariant::String:
                        case QVariant::StringList:
                        case QVariant::Date:
                        case QVariant::DateTime:
                            fieldValue = "\""+fieldValue+"\"";
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    if (expandType == ReplaceHTMLSymbols)
                        fieldValue = replaceHTMLSymbols(dataManager->fieldData(field).toString());
                    else fieldValue = dataManager->fieldData(field).toString();
                }

                context.replace(rx.cap(0),fieldValue);

            } else {
                QString error;
                if (dataManager->lastError().isEmpty()){
                    error = QString("Field %1 not found in %2 !!! ").arg(field).arg(this->objectName());
                    dataManager->putError(error);
                } else {
                    error = dataManager->lastError();
                }
                context.replace(rx.cap(0),error);
            }
        }
    }

    return context;
}

QString ContentItemDesignIntf::expandUserVariables(QString context, RenderPass pass, ExpandType expandType, DataSourceManager* dataManager)
{
    QRegExp rx(Const::VARIABLE_RX);
    if (context.contains(rx)){
        int pos = 0;
        while ((pos = rx.indexIn(context,pos))!=-1){
            QString variable=rx.cap(1);
            pos += rx.matchedLength();
            if (dataManager->containsVariable(variable) ){
                if (pass==dataManager->variablePass(variable)){
                    switch (expandType){
                    case EscapeSymbols:
                        context.replace(rx.cap(0),escapeSimbols(dataManager->variable(variable).toString()));
                    break;
                    case NoEscapeSymbols:
                        context.replace(rx.cap(0),dataManager->variable(variable).toString());
                    break;
                    case ReplaceHTMLSymbols:
                        context.replace(rx.cap(0),replaceHTMLSymbols(dataManager->variable(variable).toString()));
                    break;
                    }
                    pos=0;
                }
            } else {
                context.replace(rx.cap(0),tr("Variable %1 not found").arg(variable));
            }
        }
    }
    return context;
}

QString ContentItemDesignIntf::expandScripts(QString context, DataSourceManager* dataManager)
{
    QRegExp rx(Const::SCRIPT_RX);

    if (context.contains(rx)){
        ScriptEngineManager::instance().setDataManager(dataManager);
        QScriptEngine* se = ScriptEngineManager::instance().scriptEngine();

        QScriptValue svThis =  se->globalObject().property("THIS");
        if (svThis.isValid()){
            se->newQObject(svThis, this);
        } else {
            svThis = se->newQObject(this);
            se->globalObject().setProperty("THIS",svThis);
        }

        ScriptExtractor scriptExtractor(context);
        if (scriptExtractor.parse()){
            for(int i=0; i<scriptExtractor.count();++i){
                QString scriptBody = expandDataFields(scriptExtractor.bodyAt(i),EscapeSymbols, dataManager);
                scriptBody = expandUserVariables(scriptBody, FirstPass, EscapeSymbols, dataManager);
                QScriptValue value = se->evaluate(scriptBody);
                if (!se->hasUncaughtException()) {
                    context.replace(scriptExtractor.scriptAt(i),value.toString());
                } else {
                    context.replace(scriptExtractor.scriptAt(i),se->uncaughtException().toString());
                }
            }
        }
    }
    return context;
}

QString ContentItemDesignIntf::content() const
{
    return "";
}

QString ContentItemDesignIntf::escapeSimbols(const QString &value)
{
    QString result = value;
    result.replace("\"","\\\"");
    return result;
}

QString ContentItemDesignIntf::replaceHTMLSymbols(const QString &value)
{
    QString result = value;
    result.replace("<","&lt;");
    result.replace(">","&gt;");
    return result;
}

Spacer::Spacer(QObject *owner, QGraphicsItem *parent)
    :ItemDesignIntf("Spacer",owner,parent){}

}// namespace LimeReport
