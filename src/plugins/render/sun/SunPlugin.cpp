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

#include <QIcon>

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
    return QStringList(QStringLiteral("stars"));
}

QString SunPlugin::renderPolicy() const
{
    return QStringLiteral("SPECIFIED_ALWAYS");
}

QStringList SunPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("ALWAYS_ON_TOP"));
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
    return QStringLiteral("sun");
}

QString SunPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString SunPlugin::description() const
{
    return tr( "A plugin that shows the Sun." );
}

QString SunPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> SunPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"))
            << PluginAuthor(QStringLiteral("Harshit Jain"), QStringLiteral("hjain.itbhu@gmail.com"));
}

QIcon SunPlugin::icon () const
{
    return QIcon(MarbleDirs::path(QStringLiteral("svg/sunshine.png")));
}


void SunPlugin::initialize ()
{
    m_pixmap = QPixmap(MarbleDirs::path(QStringLiteral("svg/sunshine.png"))).scaled(QSize(22,22));
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

#include "moc_SunPlugin.cpp"
