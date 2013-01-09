//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "SunPlugin.h"

#include "GeoDataCoordinates.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "GeoPainter.h"
#include "SunLocator.h"

namespace Marble
{

SunPlugin::SunPlugin()
    : RenderPlugin( 0 )
{
}

SunPlugin::SunPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel )
{
    setVisible( false );
}

QStringList SunPlugin::backendTypes() const
{
    return QStringList( "stars" );
}

QString SunPlugin::renderPolicy() const
{
    return QString( "SPECIFIED_ALWAYS" );
}

QStringList SunPlugin::renderPosition() const
{
    QStringList layers = QStringList() << "ALWAYS_ON_TOP";
    return layers;
}

QString SunPlugin::name() const
{
    return tr( "Sun" );
}

QString SunPlugin::guiString() const
{
    return tr( "Sun" );
}

QString SunPlugin::nameId() const
{
    return QString( "sun" );
}

QString SunPlugin::version() const
{
    return "1.0";
}

QString SunPlugin::description() const
{
    return tr( "A plugin that shows the Sun." );
}

QString SunPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> SunPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de")
            << PluginAuthor( "Harshit Jain", "hjain.itbhu@gmail.com" );
}

QIcon SunPlugin::icon () const
{
    return QIcon( MarbleDirs::path( "svg/sunshine.png" ) );
}


void SunPlugin::initialize ()
{
    m_pixmap = QPixmap( MarbleDirs::path( "svg/sunshine.png" ) ).scaled( QSize(22,22) );
}

bool SunPlugin::isInitialized () const
{
    return !m_pixmap.isNull();
}

bool SunPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    if( visible() )
    {
        const qreal lon = marbleModel()->sunLocator()->getLon();
        const qreal lat = marbleModel()->sunLocator()->getLat();
        const GeoDataCoordinates coordinates( lon, lat, 0, GeoDataCoordinates::Degree );
        painter->drawPixmap( coordinates, m_pixmap );
    }

    return true;
}

}

Q_EXPORT_PLUGIN2( SunPlugin, Marble::SunPlugin )

#include "SunPlugin.moc"
