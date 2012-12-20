//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_ROUTINORUNNER_H
#define MARBLE_ROUTINORUNNER_H

#include "MarbleAbstractRunner.h"

namespace Marble
{

class RoutinoRunnerPrivate;

class RoutinoRunner : public MarbleAbstractRunner
{
    Q_OBJECT
public:
    explicit RoutinoRunner(QObject *parent = 0);

    ~RoutinoRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private:
    RoutinoRunnerPrivate* const d;
};

}

#endif
