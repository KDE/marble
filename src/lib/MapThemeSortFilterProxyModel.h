//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Claudiu Covaci <claudiu.covaci@gmail.com>
// Copyright 2009      Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_MAPTHEMESORTFILTERPROXYMODEL_H
#define MARBLE_MAPTHEMESORTFILTERPROXYMODEL_H

#include <QtGui/QSortFilterProxyModel>

class QModelIndex;

namespace Marble
{

class MapThemeSortFilterProxyModel : public QSortFilterProxyModel
{
     Q_OBJECT

    public:
    MapThemeSortFilterProxyModel(QObject *parent = 0);
    protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

};

}

#endif
