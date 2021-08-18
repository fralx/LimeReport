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
#include "lrpageheader.h"
#include "lrdesignelementsfactory.h"
#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"
#include "lrpageitemdesignintf.h"
#include "lrglobal.h"

const QString xmlTag ="PageHeader";

namespace{

LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::PageHeader(owner,parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Page Header"),LimeReport::Const::bandTAG),
        createBand
    );
}

namespace LimeReport{

PageHeader::PageHeader(QObject* owner, QGraphicsItem *parent)
: BandDesignIntf(LimeReport::BandDesignIntf::PageHeader,xmlTag,owner,parent),
  m_printOnFirstPage(true), m_printOnLastPage(true) {
    setBandTypeText(tr("Page Header"));
    setMarkerColor(bandColor());
}

BaseDesignIntf *PageHeader::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new PageHeader(owner,parent);
}

QColor PageHeader::bandColor() const
{
    return QColor(246,120,12);
}

bool PageHeader::printOnLastPage() const
{
    return m_printOnLastPage;
}

void PageHeader::setPrintOnLastPage(bool printOnLastPage)
{
    m_printOnLastPage = printOnLastPage;
}

bool PageHeader::printOnFirstPage() const
{
    return m_printOnFirstPage;
}

void PageHeader::setPrintOnFirstPage(bool printOnFirstPage)
{
    m_printOnFirstPage = printOnFirstPage;
}

}
