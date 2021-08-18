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
#ifndef LRBANDSMANAGER_H
#define LRBANDSMANAGER_H

#include <QStringList>
#include <QObject>
#include <QMap>
#include "lrdesignelementsfactory.h"
#include "lrbanddesignintf.h"
#include "lrbasedesignintf.h"

namespace LimeReport{
class BandsManager
{
public:
    BandsManager();
    QStringList bandNames();
    BandDesignIntf * createBand(const QString& type, QObject* owner, LimeReport::BaseDesignIntf*  parent );
    BandDesignIntf * createBand(BandDesignIntf::BandsType bandType, QObject* owner, LimeReport::BaseDesignIntf*  parent );
};
}
#endif // LRBANDSMANAGER_H
