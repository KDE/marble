//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Valery Kharitonov <kharvd@gmail.com>
//

#include "PostalCodePlugin.h"
#include "PostalCodeModel.h"

using namespace Marble;

PostalCodePlugin::PostalCodePlugin()
    : AbstractDataPlugin( 0 )
{
}

PostalCodePlugin::PostalCodePlugin( const MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel )
{
    setEnabled( true );
    setVisible( false );
}

void PostalCodePlugin::initialize()
{
    setModel( new PostalCodeModel( pluginManager(), this ) );
    setNumberOfItems( 20 );
}

QString PostalCodePlugin::name() const
{
    return tr( "Postal Codes" );
}

QString PostalCodePlugin::guiString() const
{
    return tr( "Postal Codes" );
}

QString PostalCodePlugin::nameId() const
{
    return "postalCode";
}

QString PostalCodePlugin::version() const
{
    return "1.0";
}

QString PostalCodePlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> PostalCodePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Valery Kharitonov", "kharvd@gmail.com" );
}

QString PostalCodePlugin::description() const
{
    return tr( "Shows postal codes of the area on the map." );
}

QIcon PostalCodePlugin::icon() const
{
    return QIcon();
}

Q_EXPORT_PLUGIN2( PostalCodePlugin, Marble::PostalCodePlugin )

#include "PostalCodePlugin.moc"
