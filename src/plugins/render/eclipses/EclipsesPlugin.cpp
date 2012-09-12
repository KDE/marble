//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ViewportParams.h"

namespace Marble
{

EclipsesPlugin::EclipsesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false )
{
}

EclipsesPlugin::EclipsesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_isInitialized( false )
{
    connect( this, SIGNAL(settingsChanged(QString)),
                   SLOT(updateSettings()) );
    connect( this, SIGNAL(enabledChanged(bool)),
                   SLOT(enableModel(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool,QString)),
                   SLOT(visibleModel(bool)) );

    setVisible( false );
    setSettings( QHash<QString, QVariant>() );
}

EclipsesPlugin::~EclipsesPlugin()
{
}

QStringList EclipsesPlugin::backendTypes() const
{
    return QStringList( "eclipses" );
}

QString EclipsesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList EclipsesPlugin::renderPosition() const
{
    return QStringList( "SURFACE" );
}

QString EclipsesPlugin::name() const
{
    return tr( "Eclipses" );
}

QString EclipsesPlugin::nameId() const
{
    return "eclipses";
}

QString EclipsesPlugin::guiString() const
{
    return tr( "E&clipses" );
}

QString EclipsesPlugin::version() const
{
    return "1.0";
}

QString EclipsesPlugin::description() const
{
    return tr( "This plugin visualizes solar eclipses." );
}

QString EclipsesPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> EclipsesPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Rene Kuettner", "rene@bitkanal.net" );
}

QIcon EclipsesPlugin::icon() const
{
    return QIcon();
}

RenderPlugin::RenderType EclipsesPlugin::renderType() const
{
    return Unknown;
}

void EclipsesPlugin::initialize()
{
    m_isInitialized = true;
    updateSettings();
    enableModel( enabled() );
}

bool EclipsesPlugin::isInitialized() const
{
    return m_isInitialized;
}

bool EclipsesPlugin::render( GeoPainter *painter,
                             ViewportParams *viewport,
                             const QString &renderPos,
                             GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );

    if( marbleModel()->planetId() == "earth" ) {
        enableModel( enabled() );
    } else {
        enableModel( false );
    }

    return true;
}

QHash<QString, QVariant> EclipsesPlugin::settings() const
{
    return m_settings;
}

void EclipsesPlugin::setSettings( const QHash<QString, QVariant> &settings )
{
    m_settings = settings;

    readSettings();
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::readSettings()
{
}

void EclipsesPlugin::writeSettings()
{
    emit settingsChanged( nameId() );
}

void EclipsesPlugin::updateSettings()
{
    if (!isInitialized()) {
        return;
    }
}

void EclipsesPlugin::enableModel( bool enabled )
{
    if ( !m_isInitialized ) {
        return;
    }
}

void EclipsesPlugin::visibleModel( bool visible )
{
    if ( !m_isInitialized ) {
        return;
    }
}

}

Q_EXPORT_PLUGIN2( EclipsesPlugin, Marble::EclipsesPlugin )

#include "EclipsesPlugin.moc"
