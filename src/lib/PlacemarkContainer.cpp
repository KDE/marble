//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "PlacemarkContainer.h"

using namespace Marble;

PlacemarkContainer::PlacemarkContainer()
{
}

PlacemarkContainer::PlacemarkContainer( const QString& name )
    : m_name( name )
{
}

PlacemarkContainer::PlacemarkContainer( const PlacemarkContainer& container )
    : QVector<GeoDataPlacemark*>( container ), m_name( container.name() )
{
}

PlacemarkContainer::PlacemarkContainer( const QVector<GeoDataPlacemark*>& container,
                                        const QString& name )
    : QVector<GeoDataPlacemark*>( container ), m_name( name )
{
}

PlacemarkContainer& PlacemarkContainer::operator= ( const PlacemarkContainer& container )
{
    QVector<GeoDataPlacemark*>::operator=( container );
    return *this;
}

inline bool populationLessThan( GeoDataPlacemark* mark1, GeoDataPlacemark* mark2 )
{ 
    // If compared items do not differ in terms of being selected,
    // compare them based on population numbers.
    if ( mark1->popularityIndex() != mark2->popularityIndex() )
        return ( mark1->popularityIndex() > mark2->popularityIndex() );
    else
        return ( mark1->popularity() > mark2->popularity() );
}


void PlacemarkContainer::setName( const QString& name )
{
    m_name = name;
}

QString PlacemarkContainer::name() const
{
    return m_name;
}

void PlacemarkContainer::sort( Qt::SortOrder order )
{
    // FIXME: use order
    Q_UNUSED( order )

    qStableSort( begin(), end(), populationLessThan );
}
