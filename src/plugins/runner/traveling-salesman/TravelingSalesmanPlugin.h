// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_TRAVELINGSALESMANPLUGIN_H
#define MARBLE_TRAVELINGSALESMANPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class TravelingSalesmanPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.TravelingSalesmanPlugin")
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit TravelingSalesmanPlugin( QObject *parent = 0 );

    virtual MarbleAbstractRunner* newRunner() const;
};

}

#endif
