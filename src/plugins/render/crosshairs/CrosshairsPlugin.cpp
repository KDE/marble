//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2010 Cezar Mocan <mocancezar@gmail.com>
//

#include "CrosshairsPlugin.h"
#include "ui_CrosshairsConfigWidget.h"

#include "AbstractProjection.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtSvg/QSvgRenderer>


namespace Marble
{

CrosshairsPlugin::CrosshairsPlugin ( )
    : m_isInitialized( false ),
      m_aboutDialog( 0 ),
      m_svgobj( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
    // nothing to do
}

CrosshairsPlugin::~CrosshairsPlugin ()
{
    delete m_svgobj;
}


QStringList CrosshairsPlugin::backendTypes() const
{
    return QStringList( "crosshairs" );
}

QString CrosshairsPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList CrosshairsPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" ); // although this is not a float item we choose the position of one
}

QString CrosshairsPlugin::name() const
{
    return tr( "Crosshairs" );
}

QString CrosshairsPlugin::guiString() const
{
    return tr( "Cross&hairs" );
}

QString CrosshairsPlugin::nameId() const
{
    return QString( "crosshairs" );
}

QString CrosshairsPlugin::description() const
{
    return tr( "A plugin that shows crosshairs." );
}

QIcon CrosshairsPlugin::icon () const
{
    return QIcon( ":/icons/crosshairs.png" );
}

QDialog* CrosshairsPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Crosshairs Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author tackat, cezar;
        
        cezar.name = QString::fromUtf8( "Cezar Mocan" );
        cezar.task = tr( "Developer" );
        cezar.email = "cezarmocan@gmail.com";
        authors.append( cezar );
        
        tackat.name = "Torsten Rahn";
        tackat.task = tr( "Developer" );
        tackat.email = "tackat@kde.org";
        authors.append( tackat );
        
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

void CrosshairsPlugin::initialize ()
{
    readSettings();
    m_isInitialized = true;
}

bool CrosshairsPlugin::isInitialized () const
{
    return m_isInitialized;
}

QDialog *CrosshairsPlugin::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::CrosshairsConfigWidget;
        m_uiConfigWidget->setupUi( m_configDialog );
        readSettings();
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL( accepted() ),
                SLOT( writeSettings() ) );
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL( rejected() ),
                SLOT( readSettings() ) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 this,        SLOT( writeSettings() ) );
    }

    return m_configDialog;
}

QHash<QString,QVariant> CrosshairsPlugin::settings() const
{
    return m_settings;
}

void CrosshairsPlugin::setSettings( QHash<QString,QVariant> settings )
{
    m_settings = settings;
    readSettings();
}


void CrosshairsPlugin::readSettings()
{
    int index = m_settings.value( "theme", 0 ).toInt();
    if ( m_uiConfigWidget && index >= 0 && index < m_uiConfigWidget->m_themeList->count() ) {
        m_uiConfigWidget->m_themeList->setCurrentRow( index );
    }

    QString theme = ":/crosshairs-pointed.svg";
    switch( index ) {
    case 1:
        theme = ":/crosshairs-gun1.svg";
        break;
    case 2:
        theme = ":/crosshairs-gun2.svg";
        break;
    case 3:
        theme = ":/crosshairs-circled.svg";
        break;
    case 4:
        theme = ":/crosshairs-german.svg";
        break;
    }

    delete m_svgobj;
    CrosshairsPlugin * me = const_cast<CrosshairsPlugin*>( this );
    m_svgobj = new QSvgRenderer( theme, me );
    m_crosshairs = QPixmap();
}

void CrosshairsPlugin::writeSettings()
{
    if ( m_uiConfigWidget ) {
        m_settings["theme"] = m_uiConfigWidget->m_themeList->currentRow();
    }
    readSettings();
    emit settingsChanged( nameId() );
}

bool CrosshairsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                               const QString& renderPos,
                               GeoSceneLayer * layer )
{
    Q_UNUSED( layer )

    if ( renderPos == "ALWAYS_ON_TOP" ) {
        const int width = 21;
        const int height = 21;

        if ( m_crosshairs.isNull() ) {
            painter->setRenderHint( QPainter::Antialiasing, true );
            m_crosshairs = QPixmap( QSize( width, height ) );
            m_crosshairs.fill( Qt::transparent );
            QPainter mapPainter( &m_crosshairs );
            mapPainter.setViewport( m_crosshairs.rect() );
            m_svgobj->render( &mapPainter );
            mapPainter.setViewport( QRect( QPoint( 0, 0 ), viewport->size() ) );
        }

        qreal centerx = 0.0;
        qreal centery = 0.0;
        viewport->currentProjection()->screenCoordinates(viewport->focusPoint(), viewport, centerx, centery);
        painter->drawPixmap( QPoint ( centerx - width / 2, centery - height / 2 ), m_crosshairs );
    }

    return true;
}

}

Q_EXPORT_PLUGIN2( CrosshairsPlugin, Marble::CrosshairsPlugin )

#include "CrosshairsPlugin.moc"
