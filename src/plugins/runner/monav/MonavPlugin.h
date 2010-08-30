//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVPLUGIN_H
#define MARBLE_MONAVPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class MonavPluginPrivate;

class MonavPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit MonavPlugin( QObject *parent = 0 );

    ~MonavPlugin();

    virtual MarbleAbstractRunner* newRunner() const;

private Q_SLOTS:
    void killProcess();

private:
    MonavPluginPrivate* const d;
};

}

#endif
