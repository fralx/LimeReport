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
#ifndef LRGROUPBANDS_H
#define LRGROUPBANDS_H

#include "lrbanddesignintf.h"
#include "lrdesignelementsfactory.h"

namespace LimeReport{

class GroupBandHeader : public BandDesignIntf, public IGroupBand{
    Q_OBJECT
    Q_PROPERTY(QString groupFieldName READ groupFieldName WRITE setGroupFieldName)
    Q_PROPERTY(bool splittable READ isSplittable WRITE setSplittable )
    Q_PROPERTY(bool keepGroupTogether READ tryToKeepTogether WRITE setTryToKeepTogether)
    Q_PROPERTY(bool startNewPage READ startNewPage WRITE setStartNewPage)
    Q_PROPERTY(bool resetPageNumber READ resetPageNumber WRITE setResetPageNumber)
    Q_PROPERTY(bool reprintOnEachPage READ reprintOnEachPage WRITE setReprintOnEachPage)
    Q_PROPERTY(QString condition READ condition WRITE setCondition)
public:
    GroupBandHeader(QObject* owner = 0, QGraphicsItem* parent=0);
    virtual bool isUnique() const;
    QVariant groupFieldValue(){return m_groupFieldValue;}
    void setGroupFieldValue(QVariant value){m_groupFieldValue=value;}
    QString groupFieldName(){return m_groupFiledName;}
    void setGroupFieldName(QString fieldName){m_groupFiledName=fieldName;}
    QColor bandColor() const;
    bool startNewPage() const;
    void setStartNewPage(bool startNewPage);
    bool resetPageNumber() const;
    void setResetPageNumber(bool resetPageNumber);
    bool isHeader() const{return true;}
    bool isGroupHeader() const {return true;}
    QString condition() const;
    void setCondition(const QString &condition);
private:
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
    void startGroup(DataSourceManager* dataManager);
    bool isNeedToClose(DataSourceManager *dataManager);
    bool isStarted();
    void closeGroup();
    int index();
    QString findDataSourceName(BandDesignIntf *parentBand);
    QString calcCondition(DataSourceManager *dataManager);
private:
    QVariant m_groupFieldValue;
    QString m_groupFiledName;
    bool m_groupStarted;
    //bool m_startNewPage;
    bool m_resetPageNumber;
    QString m_condition;
    QString m_conditionValue;
};

class GroupBandFooter : public BandDesignIntf{
    Q_OBJECT
public:
    GroupBandFooter(QObject* owner = 0, QGraphicsItem* parent=0);
    virtual bool isUnique() const;
    QColor bandColor() const;
    virtual bool isFooter() const{return true;}
private:
    virtual BaseDesignIntf* createSameTypeItem(QObject* owner=0, QGraphicsItem* parent=0);
};

} // namespace LimeReport
#endif // LRGROUPBANDS_H
