//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#include "OpenCachingComPlugin.h"
#include "OpenCachingComModel.h"

namespace Marble {

OpenCachingComPlugin::OpenCachingComPlugin()
    : AbstractDataPlugin( 0 )
{
}

OpenCachingComPlugin::OpenCachingComPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin( marbleModel )
    , m_isInitialized( false )
{
    setEnabled( true );  // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}

void OpenCachingComPlugin::initialize()
{
    OpenCachingComModel *model = new OpenCachingComModel( marbleModel(), pluginManager(), this );
    setModel( model );
    setNumberOfItems( numberOfItemsOnScreen );
    m_isInitialized = true;
}

bool OpenCachingComPlugin::isInitialized() const
{
    return m_isInitialized;
}

QStringList OpenCachingComPlugin::backendTypes() const
{
    return QStringList("opencaching.com");
}

QString OpenCachingComPlugin::name() const
{
    return tr( "OpenCaching.Com" );
}

QString OpenCachingComPlugin::guiString() const
{
    return tr( "&OpenCaching.Com" );
}

QString OpenCachingComPlugin::nameId() const
{
    return QString( "opencaching.com" );
}

QString OpenCachingComPlugin::version() const
{
    return QString( "1.0" );
}

QString OpenCachingComPlugin::description() const
{
    return tr( "Shows caches from OpenCaching.com on the map." );
}

QString OpenCachingComPlugin::copyrightYears() const
{
    return QString( "2012" );
}

QList<PluginAuthor> OpenCachingComPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
        << PluginAuthor( "Anders Lund", "anders@alweb.dk" );
}

QIcon OpenCachingComPlugin::icon() const
{
    return QIcon(":/occlogo.png");
}

}

Q_EXPORT_PLUGIN2( OpenCachingComPlugin, Marble::OpenCachingComPlugin )

#include "OpenCachingComPlugin.moc"
