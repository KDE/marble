//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
// Coprright 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy  <illya.kovalevskyy@gmail.com>
//

// Self
#include "CelestialSortFilterProxyModel.h"

namespace Marble {

CelestialSortFilterProxyModel::CelestialSortFilterProxyModel()
{
    setupPriorities();
    setupMoonsList();
    setupDwarfsList();
}

CelestialSortFilterProxyModel::~CelestialSortFilterProxyModel() {}


QVariant CelestialSortFilterProxyModel::data( const QModelIndex &index, int role ) const
{
    QVariant var = QSortFilterProxyModel::data( index, role );
    if ( role == Qt::DisplayRole && index.column() == 0 ) {
        QString newOne = var.toString();
        if (newOne == tr("Moon")) {
            return QString(QLatin1String("  ") + tr("Moon"));
        } else if ( m_moons.contains( newOne.toLower() ) ) {
            return QString(QLatin1String("  ") + newOne + QLatin1String(" (") + tr("moon") + QLatin1Char(')'));
        } else if ( m_dwarfs.contains( newOne.toLower() ) ) {
            return QString(newOne + QLatin1String(" (") + tr("dwarf planet") + QLatin1Char(')'));
        }
        return newOne;
    } else {
        return var;
    }
}


void CelestialSortFilterProxyModel::setupPriorities()
{
    // TODO: create priority on the model side (Planet Class) by taking the distance to the "home planet/home star" into account
    // here we will set m_priority for default order
    int prefix = 100;

    m_priority["sun"] = prefix;
    m_priority["mercury"] = prefix--;
    m_priority["venus"] = prefix--;
    m_priority["earth"] = prefix--;
    m_priority["moon"] = prefix--;
    m_priority["mars"] = prefix--;

    m_priority["jupiter"] = prefix--;
    // Moons of Jupiter
    m_priority["io"] = prefix--;
    m_priority["europa"] = prefix--;
    m_priority["ganymede"] = prefix--;
    m_priority["callisto"] = prefix--;

    m_priority["saturn"] = prefix--;
    // Moons of Saturn
    m_priority["mimas"] = prefix--;
    m_priority["enceladus"] = prefix--;
    m_priority["thetys"] = prefix--;
    m_priority["dione"] = prefix--;
    m_priority["rhea"] = prefix--;
    m_priority["titan"] = prefix--;
    m_priority["iapetus"] = prefix--;

    m_priority["uranus"] = prefix--;
    m_priority["neptune"] = prefix--;
    m_priority["pluto"] = prefix--;
    m_priority["ceres"] = prefix--;
}

void CelestialSortFilterProxyModel::setupMoonsList()
{
    m_moons.push_back("moon");
    m_moons.push_back("europa");
    m_moons.push_back("ganymede");
    m_moons.push_back("callisto");
    m_moons.push_back("mimas");
    m_moons.push_back("enceladus");
    m_moons.push_back("thetys");
    m_moons.push_back("dione");
    m_moons.push_back("rhea");
    m_moons.push_back("titan");
    m_moons.push_back("iapetus");
}

void CelestialSortFilterProxyModel::setupDwarfsList()
{
    m_dwarfs.push_back("pluto");
    m_dwarfs.push_back("ceres");
}

bool CelestialSortFilterProxyModel::lessThan( const QModelIndex &left, const QModelIndex &right ) const
{
    const QString nameLeft = sourceModel()->index( left.row(), 1 ).data().toString();
    const QString nameRight = sourceModel()->index( right.row(), 1 ).data().toString();
    const QString first = nameLeft.toLower();
    const QString second = nameRight.toLower();

    // both are in the list
    if ( m_priority.contains( first ) && m_priority.contains( second ) ) {
        return m_priority[first] > m_priority[second];
    }

    // only left in the list
    if ( m_priority.contains( first ) && !m_priority.contains( second ) ) {
        return true;
    }

    // only right in the list
    if (!m_priority.contains( first ) && m_priority.contains( second ) ) {
        return false;
    }

    return QSortFilterProxyModel::lessThan( left, right );
}

}

#include "moc_CelestialSortFilterProxyModel.cpp"
