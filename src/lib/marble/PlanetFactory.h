// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2009 David Roberts <dvdr18@gmail.com>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>

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

}

#endif
