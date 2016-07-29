//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
