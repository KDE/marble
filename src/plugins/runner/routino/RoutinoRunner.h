//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_ROUTINORUNNER_H
#define MARBLE_ROUTINORUNNER_H

#include "RoutingRunner.h"

namespace Marble
{

class RoutinoRunnerPrivate;

class RoutinoRunner : public RoutingRunner
{
    Q_OBJECT
public:
    explicit RoutinoRunner(QObject *parent = nullptr);

    ~RoutinoRunner() override;

    // Overriding MarbleAbstractRunner
    void retrieveRoute( const RouteRequest *request ) override;

private:
    RoutinoRunnerPrivate* const d;
};

}

#endif
