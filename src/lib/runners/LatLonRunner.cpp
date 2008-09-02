/*
    Copyright 2008 Henry de Valence <hdevalence@gmail.com>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "LatLonRunner.h"

#include "MarbleAbstractRunner.h"
#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <QtDebug>

namespace Marble
{

LatLonRunner::LatLonRunner(QObject *parent) : MarbleAbstractRunner(parent)
{
}


QString LatLonRunner::name() const
{
    return "FIXME";
}

LatLonRunner::~LatLonRunner()
{
}

void LatLonRunner::parse(const QString &input)
{
    emit runnerStarted( name() );
    
    bool successful = false;
    GeoDataCoordinates coord = GeoDataCoordinates::fromString( input, successful );
    
    GeoDataPlacemark *placemark = new GeoDataPlacemark();
    placemark->setName( input );
    qreal lon, lat;
    coord.geoCoordinates( lon, lat );
    placemark->setCoordinate( lon, lat );
    QVector<GeoDataPlacemark*> vector;
    
    //TODO: FIXME
    if( successful ) {
        //TODO: set placemark icon  
        placemark->setPopularity( 1000000000 );
        placemark->setPopularityIndex( 18 );
        vector.append( placemark );
    }
    
    emit runnerFinished( vector );    
}

}

#include "LatLonRunner.moc"
