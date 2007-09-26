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

#include "PlaceMarkContainer.h"


PlaceMarkContainer::PlaceMarkContainer()
{
}


PlaceMarkContainer::PlaceMarkContainer( const QString& name )
    : m_name(name)
{
}


inline bool populationLessThan( PlaceMark* mark1, PlaceMark* mark2 )
{ 
    // If compared items do not differ in terms of being selected,
    // compare them based on population numbers.
    return (mark1->popularity() > mark2->popularity());
}


void PlaceMarkContainer::setName( const QString& name )
{
    m_name = name;
}

QString PlaceMarkContainer::name() const
{
    return m_name;
}

void PlaceMarkContainer::sort()
{
    qStableSort( begin(), end(), populationLessThan );
}
