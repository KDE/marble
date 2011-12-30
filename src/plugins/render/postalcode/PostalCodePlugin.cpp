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
    : m_isInitialized( false )
{
    setNameId( "postalCode" );

    setVersion( "1.0" );
    setCopyrightYear( 2011 );
    addAuthor( "Valery Kharitonov", "kharvd@gmail.com" );

    setEnabled( true );
    setVisible( false );
}

void PostalCodePlugin::initialize()
{
    setModel( new PostalCodeModel( pluginManager(), this ) );
    setNumberOfItems( numberOfItemsOnScreen );

    m_isInitialized = true;
}

bool PostalCodePlugin::isInitialized() const
{
    return m_isInitialized;
}

QString PostalCodePlugin::name() const
{
    return tr( "Postal Codes" );
}

QString PostalCodePlugin::guiString() const
{
    return tr( "Postal Codes" );
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
