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
#include "lrpagefooter.h"
#include "lrdesignelementsfactory.h"
#include "lrglobal.h"

const QString xmlTag ="PageFooter";

namespace{
LimeReport::BaseDesignIntf * createBand(QObject* owner, LimeReport::BaseDesignIntf*  parent){
    return new LimeReport::PageFooter(owner,parent);
}

bool VARIABLE_IS_NOT_USED registred = LimeReport::DesignElementsFactory::instance().registerCreator(
        xmlTag,
        LimeReport::ItemAttribs(QObject::tr("Page Footer"),LimeReport::Const::bandTAG),
        createBand
    );
}

namespace LimeReport{

PageFooter::PageFooter(QObject *owner, QGraphicsItem *parent)
    : BandDesignIntf(LimeReport::BandDesignIntf::PageFooter,xmlTag,owner,parent),
      m_printOnFirstPage(true), m_printOnLastPage(true)
{
        setBandTypeText( tr("Page Footer") );
        setMarkerColor(bandColor());
        setAutoHeight(false);
}

BaseDesignIntf *PageFooter::createSameTypeItem(QObject *owner, QGraphicsItem *parent)
{
    return new PageFooter(owner,parent);
}

QColor PageFooter::bandColor() const
{
   return QColor(246,120,12);
}

bool PageFooter::printOnFirstPage() const
{
    return m_printOnFirstPage;
}

void PageFooter::setPrintOnFirstPage(bool printOnFirstPage)
{
    m_printOnFirstPage = printOnFirstPage;
}

bool PageFooter::printOnLastPage() const
{
    return m_printOnLastPage;
}

void PageFooter::setPrintOnLastPage(bool printOnLastPage)
{
    m_printOnLastPage = printOnLastPage;
}

} // namespace LimeReport
