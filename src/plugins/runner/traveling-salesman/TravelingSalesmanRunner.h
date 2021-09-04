//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_TRAVELINGSALESMANRUNNER_H
#define MARBLE_TRAVELINGSALESMANRUNNER_H

#include "MarbleAbstractRunner.h"
#include "routing/RouteRequest.h"

namespace Marble
{

class TravelingSalesmanRunnerPrivate;

class TravelingSalesmanRunner : public MarbleAbstractRunner
{
public:
    explicit TravelingSalesmanRunner( QObject *parent = 0 );

    ~TravelingSalesmanRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( RouteRequest *request );

private:
    TravelingSalesmanRunnerPrivate* const d;
};

}

#endif
