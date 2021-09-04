// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Valery Kharitonov <kharvd@gmail.com>
//

#include "PostalCodePlugin.h"
#include "PostalCodeModel.h"

#include <QIcon>

using namespace Marble;

PostalCodePlugin::PostalCodePlugin()
    : AbstractDataPlugin( nullptr )
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
    setModel( new PostalCodeModel( marbleModel(), this ) );
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
    return QStringLiteral("postalCode");
}

QString PostalCodePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString PostalCodePlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> PostalCodePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Valery Kharitonov"), QStringLiteral("kharvd@gmail.com"));
}

QString PostalCodePlugin::description() const
{
    return tr( "Shows postal codes of the area on the map." );
}

QIcon PostalCodePlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/postalcode.png"));
}

#include "moc_PostalCodePlugin.cpp"
