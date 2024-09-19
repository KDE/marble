// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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
    explicit MonavRunner(const MonavPlugin *plugin, QObject *parent = nullptr);

    ~MonavRunner() override;

    // Overriding MarbleAbstractRunner
    void retrieveRoute(const RouteRequest *request) override;

#if 0
    // Overriding MarbleAbstractRunner
    void reverseGeocoding( const GeoDataCoordinates &coordinates );
#endif

private:
    MonavRunnerPrivate *const d;
};

}

#endif
