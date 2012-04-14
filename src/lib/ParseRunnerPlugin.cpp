//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "ParseRunnerPlugin.h"

namespace Marble
{

class ParseRunnerPlugin::Private
{
public:
    Private();
};

ParseRunnerPlugin::Private::Private()
{
    // nothing to do
}

ParseRunnerPlugin::ParseRunnerPlugin( QObject* parent ) :
    QObject( parent ),
    d( new Private )
{
}

ParseRunnerPlugin::~ParseRunnerPlugin()
{
    delete d;
}

QIcon ParseRunnerPlugin::icon() const
{
    return QIcon();
}

}

#include "ParseRunnerPlugin.moc"
