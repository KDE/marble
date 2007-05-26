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

#include "placecontainer.h"


PlaceContainer::PlaceContainer()
{
    clear();
}


PlaceContainer::PlaceContainer( const QString& name )
    : m_name(name)
{
    clear();
}
