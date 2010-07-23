//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "MarbleAbstractRunner.h"
#include "MarbleMap.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Marble
{

MarbleAbstractRunner::MarbleAbstractRunner( QObject *parent )
    : QObject( parent ),
      m_map( 0 )
{
    // nothing to do
}

GeoDataFeature::GeoDataVisualCategory MarbleAbstractRunner::category() const
{
    return GeoDataFeature::Default;
}

void MarbleAbstractRunner::setMap( MarbleMap* map )
{
    m_map = map;
}

MarbleMap * MarbleAbstractRunner::map()
{
    return m_map;
}

void MarbleAbstractRunner::search( const QString & )
{
    // dummy implementation
    emit searchFinished( QVector<GeoDataPlacemark*>() );
}

void MarbleAbstractRunner::reverseGeocoding( const GeoDataCoordinates & )
{
    // dummy implementation
    emit reverseGeocodingFinished( GeoDataPlacemark() );
}

void MarbleAbstractRunner::retrieveRoute( RouteSkeleton* )
{
    // dummy implementation
    emit routeCalculated( 0 );
}

}

#include "MarbleAbstractRunner.moc"
