//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

#ifndef MARBLE_LOCALOSMSEARCHRUNNER_H
#define MARBLE_LOCALOSMSEARCHRUNNER_H

#include "MarbleAbstractRunner.h"
#include "OsmPlacemark.h"
#include "GeoDataFeature.h"

#include <QtCore/QString>

namespace Marble
{

class OsmDatabase;

class LocalOsmSearchRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit LocalOsmSearchRunner( OsmDatabase *m_database, QObject *parent = 0 );

    ~LocalOsmSearchRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    virtual void search( const QString &searchTerm, const GeoDataLatLonAltBox preferred );

private:
    OsmDatabase *m_database;

    static QMap<OsmPlacemark::OsmCategory, GeoDataFeature::GeoDataVisualCategory> m_categoryMap;
};

}

#endif
