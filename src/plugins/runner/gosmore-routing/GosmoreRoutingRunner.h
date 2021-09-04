//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
    explicit GosmoreRunner(QObject *parent = nullptr);

    ~GosmoreRunner() override;

    // Overriding MarbleAbstractRunner
    void retrieveRoute( const RouteRequest *request ) override;

private:
    GosmoreRunnerPrivate* const d;
};

}

#endif
