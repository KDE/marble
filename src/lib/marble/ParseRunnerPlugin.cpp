// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "ParseRunnerPlugin.h"

#include <QIcon>

namespace Marble
{

class Q_DECL_HIDDEN ParseRunnerPlugin::Private
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

#include "moc_ParseRunnerPlugin.cpp"
