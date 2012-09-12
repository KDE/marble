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
#include "MarbleClock.h"
#include "ViewportParams.h"

#include "EclipsesModel.h"

namespace Marble
{

EclipsesPlugin::EclipsesPlugin()
    : RenderPlugin( 0 ),
      m_isInitialized( false ),
      m_model( 0 )
{
}

EclipsesPlugin::EclipsesPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
     m_isInitialized( false ),
     m_model( 0 )
{
    connect( this, SIGNAL(settingsChanged(QString)),
                   SLOT(updateSettings()) );

    setSettings( QHash<QString, QVariant>() );
    setEnabled( true );
}

EclipsesPlugin::~EclipsesPlugin()
{
    if( m_isInitialized ) {
        delete m_model;
    }
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
    return QStringList( "FLOAT_ITEM" );
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
    if( isInitialized() ) {
        return;
    }

 
    const MarbleClock *clock = marbleModel()->clock();
    m_model = new EclipsesModel( clock );

    updateEclipses();

    connect( clock, SIGNAL( timeChanged() ), SLOT( updateEclipses() ) );

    m_isInitialized = true;
    updateSettings();
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
        // render
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

void EclipsesPlugin::updateEclipses()
{
    mDebug() << "Updating eclipses....";
    const MarbleClock *clock = marbleModel()->clock();
    QDateTime currentDT = clock->dateTime();
    m_model->setYear( currentDT.date().year() );
}

}

Q_EXPORT_PLUGIN2( EclipsesPlugin, Marble::EclipsesPlugin )

#include "EclipsesPlugin.moc"

