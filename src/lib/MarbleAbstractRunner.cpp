//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "MarbleAbstractRunner.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Marble
{

MarbleAbstractRunner::MarbleAbstractRunner( QObject *parent )
    : QObject( parent ),
      m_model( 0 )
{
    // nothing to do
}

GeoDataFeature::GeoDataVisualCategory MarbleAbstractRunner::category() const
{
    return GeoDataFeature::Default;
}

void MarbleAbstractRunner::setModel( MarbleModel* model )
{
    m_model = model;
}

MarbleModel * MarbleAbstractRunner::model()
{
    return m_model;
}

void MarbleAbstractRunner::search( const QString & )
{
    // dummy implementation
    emit searchFinished( QVector<GeoDataPlacemark*>() );
}

void MarbleAbstractRunner::reverseGeocoding( const GeoDataCoordinates &coordinates )
{
    // dummy implementation
    emit reverseGeocodingFinished( coordinates, GeoDataPlacemark() );
}

void MarbleAbstractRunner::retrieveRoute( RouteRequest* )
{
    // dummy implementation
    emit routeCalculated( 0 );
}

}

#include "MarbleAbstractRunner.moc"
