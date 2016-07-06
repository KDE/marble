//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Henry de Valence <hdevalence@gmail.com>
// Copyright 2009 David Roberts <dvdr18@gmail.com>
// Copyright 2012 Mohammed Nafees <nafees.technocool@gmail.com>
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>

#ifndef MARBLE_PLANETFACTORY_H
#define MARBLE_PLANETFACTORY_H

#include "marble_export.h"

#include <QList>

class QString;

namespace Marble
{

class Planet;

/**
 * @brief The PlanetFactory class provides static methods to construct any planet known to Marble
 */
class MARBLE_EXPORT PlanetFactory
{
public:
    /** Provides a list of known planet IDs */
    static QList<QString> planetList();

    /** Creates the planet with the given ID, or one with default values if ID is not among planetList() */
    static Planet construct( const QString &id );

    /** Returns a localized name of the planet with the given ID */
    static QString localizedName( const QString &id );
};

typedef PlanetFactory Magrathea;

}

#endif
