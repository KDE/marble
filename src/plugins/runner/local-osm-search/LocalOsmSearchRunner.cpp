//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

#include "LocalOsmSearchRunner.h"

#include "OsmDatabase.h"
#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>

namespace Marble
{

LocalOsmSearchRunner::LocalOsmSearchRunner( OsmDatabase *database, QObject *parent ) :
    MarbleAbstractRunner( parent ),
    m_database( database )
{
    // nothing to do
}

LocalOsmSearchRunner::~LocalOsmSearchRunner()
{
}

GeoDataFeature::GeoDataVisualCategory LocalOsmSearchRunner::category() const
{
    return GeoDataFeature::Coordinate;
}


void LocalOsmSearchRunner::search( const QString &searchTerm )
{
    QList<OsmPlacemark> placemarks = m_database->find( searchTerm );

    QVector<GeoDataPlacemark*> result;
    foreach( const OsmPlacemark &placemark, placemarks ) {
        GeoDataPlacemark* hit = new GeoDataPlacemark;
        hit->setName( placemark.name() );
        GeoDataCoordinates coordinate( placemark.longitude(), placemark.latitude(), 0.0, GeoDataCoordinates::Degree );
        hit->setCoordinate( coordinate );
        result << hit;
    }

    emit searchFinished( result );
}

} // namespace Marble

#include "LocalOsmSearchRunner.moc"
