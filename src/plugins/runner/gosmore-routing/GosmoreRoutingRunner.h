//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#ifndef MARBLE_GOSMOREROUTINGRUNNER_H
#define MARBLE_GOSMOREROUTINGRUNNER_H

#include "RoutingRunner.h"

namespace Marble
{

class GosmoreRunnerPrivate;
class RouteRequest;

class GosmoreRunner : public RoutingRunner
{
public:
    explicit GosmoreRunner(QObject *parent = 0);

    ~GosmoreRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private:
    GosmoreRunnerPrivate* const d;
};

}

#endif
