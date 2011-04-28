//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_LOCALOSMSEARCHPLUGIN_H
#define MARBLE_LOCALOSMSEARCHPLUGIN_H

#include "RunnerPlugin.h"
#include "OsmDatabase.h"

namespace Marble
{

class LocalOsmSearchPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit LocalOsmSearchPlugin( QObject *parent = 0 );

    virtual MarbleAbstractRunner* newRunner() const;

private:
    /** @todo: FIXME newRunner() is another virtual method that shouldn't be const */
    mutable bool m_databaseLoaded;
    mutable OsmDatabase m_database;
};

}

#endif
