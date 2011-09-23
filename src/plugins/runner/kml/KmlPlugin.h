//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEKMLPLUGIN_H
#define MARBLEKMLPLUGIN_H

#include "RunnerPlugin.h"
namespace Marble
{

class KmlPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit KmlPlugin( QObject *parent = 0 );

    virtual MarbleAbstractRunner* newRunner() const;
};

}
#endif // MARBLEKMLPLUGIN_H
