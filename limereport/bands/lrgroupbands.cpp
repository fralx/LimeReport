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
#include "lrgroupbands.h"
#include "lrglobal.h"
#include "lrdatasourcemanager.h"

const QString xmlTagHeader = QLatin1String("GroupHeader");
const QString xmlTagFooter = QLatin1String("GroupFooter");

namespace{

LimeReport::BaseDesignIntf* createHeader(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::GroupBandHeader(owner,parent);
}

bool VARIABLE_IS_NOT_USED registredHeader = LimeReport::DesignElementsFactory::instance().registerCreator(
       xmlTagHeader,
        LimeReport::ItemAttribs(QObject::tr("GroupHeader"),LimeReport::Const::bandTAG),
        createHeader
    );

LimeReport::BaseDesignIntf * createFooter(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::GroupBandFooter(owner,parent);
}

bool VARIABLE_IS_NOT_USED registredFooter = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTagFooter,
        LimeReport::ItemAttribs(QObject::tr("GroupFooter"),LimeReport::Const::bandTAG),
        createFooter
    );

}

namespace LimeReport{

GroupBandHeader::GroupBandHeader(QObject *owner, QGraphicsItem *parent)
    : BandDesignIntf(BandDesignIntf::GroupHeader, xmlTagHeader, owner,parent),
      m_groupFiledName(""), m_groupStarted(false), m_resetPageNumber(false)
{
    setBandTypeText(tr("GroupHeader"));
    setFixedPos(false);
    setMarkerColor(bandColor());
}

bool GroupBandHeader::isUnique() const
{
    return false;
}

//bool GroupBandHeader::tryToKeepTogether()
//{
//    return m_tryToKeepTogether;
//}

BaseDesignIntf *GroupBandHeader::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new GroupBandHeader(owner, parent);
}

void GroupBandHeader::startGroup(DataSourceManager* dataManager)
{
    m_groupStarted=true;

    QString lineVar = QLatin1String("line_")+objectName().toLower();
    dataManager->setReportVariable(lineVar,1);

    QString datasourceName = findDataSourceName(parentBand());
    if (dataManager->containsDatasource(datasourceName)){
        IDataSource* ds = dataManager->dataSource(datasourceName);
        if (ds && ds->columnIndexByName(m_groupFiledName)!=-1)
            m_groupFieldValue=ds->data(m_groupFiledName);
    }

    if (!m_condition.isEmpty()) m_conditionValue = calcCondition(dataManager);
}

QColor GroupBandHeader::bandColor() const
{
    return QColor(Qt::darkBlue);
}

QString GroupBandHeader::findDataSourceName(BandDesignIntf* parentBand){
    if (!parentBand) return "";
    if (!parentBand->datasourceName().isEmpty())
        return parentBand->datasourceName();
    else
        return findDataSourceName(parentBand->parentBand());

}

QString GroupBandHeader::condition() const
{
    return m_condition;
}

void GroupBandHeader::setCondition(const QString &condition)
{
    m_condition = condition;
}

QString GroupBandHeader::calcCondition(DataSourceManager* dataManager){
    QString result = m_condition;
    if (!m_condition.isEmpty()){
        result=expandUserVariables(result, FirstPass, NoEscapeSymbols, dataManager);
        result=expandScripts(result, dataManager);
        result=expandDataFields(result, NoEscapeSymbols, dataManager);
    }
    return result;
}

bool GroupBandHeader::isNeedToClose(DataSourceManager* dataManager)
{
    if (!m_groupStarted) return false;
    if ((m_groupFiledName.isNull() || m_groupFiledName.isEmpty()) && condition().isEmpty()){
        dataManager->putError(tr("Group field not found"));
        return false;
    }

    if (!m_condition.isEmpty()){
        return m_conditionValue != calcCondition(dataManager);
    } else {
        QString datasourceName = findDataSourceName(parentBand());
        if (dataManager->containsDatasource(datasourceName)){
            IDataSource* ds = dataManager->dataSource(datasourceName);
            if (ds){
                if (ds->data(m_groupFiledName).isNull() && m_groupFieldValue.isNull()) return false;
                if (!ds->data(m_groupFiledName).isValid()) return false;
                return ds->data(m_groupFiledName)!=m_groupFieldValue;
            }
        } else {
            dataManager->putError(tr("Datasource \"%1\" not found!").arg(datasourceName));
        }
    }

    return false;
}

bool GroupBandHeader::isStarted()
{
    return m_groupStarted;//!m_groupFieldValue.isNull();
}

void GroupBandHeader::closeGroup()
{
    m_groupFieldValue=QVariant();
    m_conditionValue="";
    m_groupStarted=false;
}

int GroupBandHeader::index()
{
    return bandIndex();
}

bool GroupBandHeader::startNewPage() const
{
    return BandDesignIntf::startNewPage();
}

void GroupBandHeader::setStartNewPage(bool startNewPage)
{
    BandDesignIntf::setStartNewPage(startNewPage);
}

bool GroupBandHeader::resetPageNumber() const
{
    return m_resetPageNumber;
}

void GroupBandHeader::setResetPageNumber(bool resetPageNumber)
{
    m_resetPageNumber = resetPageNumber;
}

GroupBandFooter::GroupBandFooter(QObject *owner, QGraphicsItem *parent)
    :BandDesignIntf(BandDesignIntf::GroupFooter, xmlTagFooter, owner,parent)
{
    setBandTypeText(tr("GroupFooter"));
    setFixedPos(false);
    setMarkerColor(bandColor());
}

bool GroupBandFooter::isUnique() const
{
    return false;
}

QColor GroupBandFooter::bandColor() const
{
    return QColor(Qt::darkBlue);
}

BaseDesignIntf *GroupBandFooter::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new GroupBandFooter(owner,parent);
}

} //namespace LimeReport
