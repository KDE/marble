// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

// Self
#include "CelestialSortFilterProxyModel.h"

namespace Marble
{

CelestialSortFilterProxyModel::CelestialSortFilterProxyModel()
{
    setupPriorities();
    setupMoonsList();
    setupDwarfsList();
}

CelestialSortFilterProxyModel::~CelestialSortFilterProxyModel() = default;

QVariant CelestialSortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    QVariant var = QSortFilterProxyModel::data(index, role);
    if (role == Qt::DisplayRole && index.column() == 0) {
        QString newOne = var.toString();
        if (newOne == tr("Moon")) {
            return QString(QLatin1StringView("  ") + tr("Moon"));
        } else if (m_moons.contains(newOne.toLower())) {
            return QString(QLatin1StringView("  ") + newOne + QLatin1StringView(" (") + tr("moon") + QLatin1Char(')'));
        } else if (m_dwarfs.contains(newOne.toLower())) {
            return QString(newOne + QLatin1StringView(" (") + tr("dwarf planet") + QLatin1Char(')'));
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

    m_priority[QStringLiteral("sun")] = prefix;
    m_priority[QStringLiteral("mercury")] = prefix--;
    m_priority[QStringLiteral("venus")] = prefix--;
    m_priority[QStringLiteral("earth")] = prefix--;
    m_priority[QStringLiteral("moon")] = prefix--;
    m_priority[QStringLiteral("mars")] = prefix--;

    m_priority[QStringLiteral("jupiter")] = prefix--;
    // Moons of Jupiter
    m_priority[QStringLiteral("io")] = prefix--;
    m_priority[QStringLiteral("europa")] = prefix--;
    m_priority[QStringLiteral("ganymede")] = prefix--;
    m_priority[QStringLiteral("callisto")] = prefix--;

    m_priority[QStringLiteral("saturn")] = prefix--;
    // Moons of Saturn
    m_priority[QStringLiteral("mimas")] = prefix--;
    m_priority[QStringLiteral("enceladus")] = prefix--;
    m_priority[QStringLiteral("thetys")] = prefix--;
    m_priority[QStringLiteral("dione")] = prefix--;
    m_priority[QStringLiteral("rhea")] = prefix--;
    m_priority[QStringLiteral("titan")] = prefix--;
    m_priority[QStringLiteral("iapetus")] = prefix--;

    m_priority[QStringLiteral("uranus")] = prefix--;
    m_priority[QStringLiteral("neptune")] = prefix--;
    m_priority[QStringLiteral("pluto")] = prefix--;
    m_priority[QStringLiteral("ceres")] = prefix--;
}

void CelestialSortFilterProxyModel::setupMoonsList()
{
    m_moons.push_back(QStringLiteral("moon"));
    m_moons.push_back(QStringLiteral("europa"));
    m_moons.push_back(QStringLiteral("ganymede"));
    m_moons.push_back(QStringLiteral("callisto"));
    m_moons.push_back(QStringLiteral("mimas"));
    m_moons.push_back(QStringLiteral("enceladus"));
    m_moons.push_back(QStringLiteral("thetys"));
    m_moons.push_back(QStringLiteral("dione"));
    m_moons.push_back(QStringLiteral("rhea"));
    m_moons.push_back(QStringLiteral("titan"));
    m_moons.push_back(QStringLiteral("iapetus"));
}

void CelestialSortFilterProxyModel::setupDwarfsList()
{
    m_dwarfs.push_back(QStringLiteral("pluto"));
    m_dwarfs.push_back(QStringLiteral("ceres"));
}

bool CelestialSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const QString nameLeft = sourceModel()->index(left.row(), 1).data().toString();
    const QString nameRight = sourceModel()->index(right.row(), 1).data().toString();
    const QString first = nameLeft.toLower();
    const QString second = nameRight.toLower();

    // both are in the list
    if (m_priority.contains(first) && m_priority.contains(second)) {
        return m_priority[first] > m_priority[second];
    }

    // only left in the list
    if (m_priority.contains(first) && !m_priority.contains(second)) {
        return true;
    }

    // only right in the list
    if (!m_priority.contains(first) && m_priority.contains(second)) {
        return false;
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

}

#include "moc_CelestialSortFilterProxyModel.cpp"
