//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_LOCALOSMSEARCHRUNNER_H
#define MARBLE_LOCALOSMSEARCHRUNNER_H

#include "SearchRunner.h"

#include "OsmDatabase.h"
#include "OsmPlacemark.h"
#include "GeoDataPlacemark.h"

#include <QMap>

namespace Marble
{

class LocalOsmSearchRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit LocalOsmSearchRunner( const QStringList &databaseFiles, QObject *parent = nullptr );

    ~LocalOsmSearchRunner() override;

    void search( const QString &searchTerm, const GeoDataLatLonBox &preferred ) override;

private:
    OsmDatabase m_database;

    static QMap<OsmPlacemark::OsmCategory, GeoDataPlacemark::GeoDataVisualCategory> m_categoryMap;
};

}

#endif
