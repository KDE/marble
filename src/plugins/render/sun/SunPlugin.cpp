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
#include "MarbleModel.h"
#include "PluginAboutDialog.h"

namespace Marble
{

SunPlugin::SunPlugin() : m_aboutDialog( 0 )
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

QString SunPlugin::description() const
{
    return tr( "A plugin that shows the Sun." );
}

QIcon SunPlugin::icon () const
{
    return QIcon( MarbleDirs::path( "svg/sunshine.png" ) );
}


void SunPlugin::initialize ()
{
    m_pixmap = QPixmap( MarbleDirs::path( "svg/sunshine.png" ) ).scaled( QSize( 30, 30 ) );
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

QDialog *SunPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Sun Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br/>(c) 2011 The Marble Project<br /><br/><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );

        QList<Author> authors;
        Author tackat;
        tackat.name = QString::fromUtf8( "Torsten Rahn" );
        tackat.task = tr( "Developer" );
        tackat.email = "tackat@kde.org";
        authors.append( tackat );
        Author bernhard;
        bernhard.name = QString::fromUtf8( "Bernhard Beschow" );
        bernhard.task = tr( "Developer" );
        bernhard.email = "bbeschow@cs.tu-berlin.de";
        authors.append( bernhard );
        Author harshit;
        harshit.name = QString::fromUtf8( "Harshit Jain" );
        harshit.task = tr( "Developer" );
        harshit.email = "hjain.itbhu@gmail.com";
        authors.append( harshit );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

}

Q_EXPORT_PLUGIN2( SunPlugin, Marble::SunPlugin )

#include "SunPlugin.moc"
