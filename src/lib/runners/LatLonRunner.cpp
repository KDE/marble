//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "LatLonRunner.h"

#include "MarbleAbstractRunner.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"

#include "MarbleDebug.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtCore/QtDebug>

namespace Marble
{

LatLonRunner::LatLonRunner(QObject *parent) : MarbleAbstractRunner(parent)
{
}


GeoDataFeature::GeoDataVisualCategory LatLonRunner::category() const
{
    return GeoDataFeature::Coordinate;
}

LatLonRunner::~LatLonRunner()
{
    wait(50); // give the thread some time to shut down cleanly
}

void LatLonRunner::run()
{
    bool successful = false;
    GeoDataCoordinates coord = GeoDataCoordinates::fromString( m_input, successful );

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setName( m_input );
    qreal lon, lat;
    coord.geoCoordinates( lon, lat );
    placemark->setCoordinate( lon, lat );
    QVector<GeoDataPlacemark*> vector;

    if( successful ) {
        placemark->setVisualCategory( category() );
        placemark->setPopularity( 1000000000 );
        placemark->setPopularityIndex( 18 );
        vector.append( placemark );
    }

    emit runnerFinished( this, vector );    
}

}

#include "LatLonRunner.moc"
