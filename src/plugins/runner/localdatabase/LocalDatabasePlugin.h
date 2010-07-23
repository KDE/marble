//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_LOCALDATABASEPLUGIN_H
#define MARBLE_LOCALDATABASEPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class LocalDatabasePlugin : public RunnerPlugin
{
    Q_OBJECT
public:
    explicit LocalDatabasePlugin(QObject *parent = 0);

    virtual MarbleAbstractRunner* newRunner() const;
};

}

#endif
