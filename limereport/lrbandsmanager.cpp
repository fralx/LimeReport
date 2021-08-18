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
#include "lrbandsmanager.h"
#include "lrglobal.h"

#include "lrpageheader.h"
#include "lrpagefooter.h"
#include "lrreportheader.h"
#include "lrreportfooter.h"
#include "lrdataband.h"
#include "lrsubdetailband.h"
#include "lrgroupbands.h"
#include "lrtearoffband.h"

namespace LimeReport{

BandsManager::BandsManager(){}

QStringList BandsManager::bandNames()
{
    QStringList bandsList;
    bandsList<<QObject::tr("Report Header")<<QObject::tr("Page Header")
            <<QObject::tr("Page Footer")<<QObject::tr("Report Footer")
            <<QObject::tr("Data")<<QObject::tr("SubDetail");

    foreach(ItemAttribs attr,LimeReport::DesignElementsFactory::instance().attribsMap().values()){
        if ((attr.m_tag==LimeReport::Const::bandTAG)&&(!bandsList.contains(attr.m_alias)))
            bandsList.append(attr.m_alias);
    }

    return bandsList;
}

BandDesignIntf* BandsManager::createBand(const QString &type, QObject *owner, LimeReport::BaseDesignIntf *parent)
{
    QString identity = LimeReport::DesignElementsFactory::instance().attribsMap().key(LimeReport::ItemAttribs(type,LimeReport::Const::bandTAG));
    return dynamic_cast<LimeReport::BandDesignIntf*>(LimeReport::DesignElementsFactory::instance().objectCreator(identity)(owner,parent));
}

BandDesignIntf *BandsManager::createBand(BandDesignIntf::BandsType bandType, QObject *owner, BaseDesignIntf *parent)
{
    switch(bandType){
    case BandDesignIntf::ReportHeader:
        return new ReportHeader(owner,parent);
    case BandDesignIntf::ReportFooter:
        return new ReportFooter(owner,parent);
    case BandDesignIntf::PageHeader:
        return new PageHeader(owner,parent);
    case BandDesignIntf::PageFooter:
        return new PageFooter(owner,parent);
    case BandDesignIntf::Data:
        return new DataBand(owner,parent);
    case BandDesignIntf::SubDetailBand:
        return new SubDetailBand(owner,parent);
    case BandDesignIntf::SubDetailHeader:
        return new SubDetailHeaderBand(owner,parent);
    case BandDesignIntf::SubDetailFooter:
        return new SubDetailFooterBand(owner,parent);
    case BandDesignIntf::GroupHeader:
        return new GroupBandHeader(owner, parent);
    case BandDesignIntf::GroupFooter:
        return new GroupBandFooter(owner, parent);
    case BandDesignIntf::DataHeader:
        return new DataHeaderBand(owner, parent);
    case BandDesignIntf::DataFooter:
        return new DataFooterBand(owner, parent);
    case BandDesignIntf::TearOffBand:
        return new TearOffBand(owner, parent);
    }

    return 0;
}

}
