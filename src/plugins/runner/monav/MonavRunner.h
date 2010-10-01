//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVRUNNER_H
#define MARBLE_MONAVRUNNER_H

#include "MarbleAbstractRunner.h"
#include "routing/RouteRequest.h"

class QProcess;

namespace Marble
{

class MonavRunnerPrivate;

class MonavRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit MonavRunner( QObject *parent = 0 );

    ~MonavRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( RouteRequest *request );

    // Overriding MarbleAbstractRunner
    void reverseGeocoding( const GeoDataCoordinates &coordinates );

private:
    MonavRunnerPrivate* const d;
};

}

#endif
