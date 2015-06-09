//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MONAVRUNNER_H
#define MARBLE_MONAVRUNNER_H

#include "RoutingRunner.h"

namespace Marble
{

class MonavRunnerPrivate;
class MonavPlugin;

class MonavRunner : public RoutingRunner
{
    Q_OBJECT
public:
    explicit MonavRunner( const MonavPlugin* plugin, QObject *parent = 0 );

    ~MonavRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

#if 0
    // Overriding MarbleAbstractRunner
    void reverseGeocoding( const GeoDataCoordinates &coordinates );
#endif

private:
    MonavRunnerPrivate* const d;
};

}

#endif
