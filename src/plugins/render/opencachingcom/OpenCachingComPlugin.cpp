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

#include <QIcon>

namespace Marble {

OpenCachingComPlugin::OpenCachingComPlugin()
    : AbstractDataPlugin( 0 )
    , m_isInitialized( false )
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
    OpenCachingComModel *model = new OpenCachingComModel( marbleModel(), this );
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
    return QStringList(QStringLiteral("opencaching.com"));
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
    return QStringLiteral("opencaching.com");
}

QString OpenCachingComPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OpenCachingComPlugin::description() const
{
    return tr( "Shows caches from OpenCaching.com on the map." );
}

QString OpenCachingComPlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> OpenCachingComPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
        << PluginAuthor(QStringLiteral("Anders Lund"), QStringLiteral("anders@alweb.dk"));
}

QIcon OpenCachingComPlugin::icon() const
{
    return QIcon(QStringLiteral(":/occlogo.png"));
}

}

#include "moc_OpenCachingComPlugin.cpp"
