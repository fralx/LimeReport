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
#include "lrreportheader.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"

const QString xmlTag ="ReportHeader";

namespace{
LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::ReportHeader(owner,parent);
}
bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Report Header"),LimeReport::Const::bandTAG),
        createBand
    );
}
namespace LimeReport {

ReportHeader::ReportHeader(QObject *owner, QGraphicsItem *parent)
    : BandDesignIntf(LimeReport::BandDesignIntf::ReportHeader,xmlTag,owner,parent), m_printBeforePageHeader(false) {
        setBandTypeText(tr("Report Header"));
        setMarkerColor(bandColor());
}
BaseDesignIntf *ReportHeader::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new ReportHeader(owner,parent);
}

QColor ReportHeader::bandColor() const
{
    return QColor(152,69,167);
}

bool ReportHeader::printBeforePageHeader() const
{
    return m_printBeforePageHeader;
}

void ReportHeader::setPrintBeforePageHeader(bool printBeforePageHeader)
{
    if (m_printBeforePageHeader != printBeforePageHeader){
        m_printBeforePageHeader = printBeforePageHeader;
        notify("printBeforePageHeader",!m_printBeforePageHeader,m_printBeforePageHeader);
    }
}

}

