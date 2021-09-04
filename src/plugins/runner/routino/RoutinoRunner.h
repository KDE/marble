//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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
