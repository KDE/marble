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

#include <QSortFilterProxyModel>

#ifndef MARBLE_CELESTIALSORTFILTERPROXYMODEL_H
#define MARBLE_CELESTIALSORTFILTERPROXYMODEL_H

namespace Marble {

/**
 * @brief The CelestialSortFilterProxyModel class is a proxy used by both
 * MapViewWidget's listview, and MapChangeEditDialog's listview.
 */
class CelestialSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CelestialSortFilterProxyModel();
    ~CelestialSortFilterProxyModel();
    /**
     * @brief A small trick to change names for dwarfs and moons
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private:
    void setupPriorities();
    void setupMoonsList();
    void setupDwarfsList();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QMap<QString, int> m_priority;
    QList<QString> m_moons;
    QList<QString> m_dwarfs;
};
}

#endif
