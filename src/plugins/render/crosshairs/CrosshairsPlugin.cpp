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

CrosshairsPlugin::CrosshairsPlugin()
    : RenderPlugin( 0 ),
      m_svgobj( 0 )
{
}

CrosshairsPlugin::CrosshairsPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_isInitialized( false ),
      m_svgobj( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
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
    return QStringList( "FLOAT_ITEM" ); // although this is not a float item we choose the position of one
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

QString CrosshairsPlugin::version() const
{
    return "1.0";
}

QString CrosshairsPlugin::description() const
{
    return tr( "A plugin that shows crosshairs." );
}

QString CrosshairsPlugin::copyrightYears() const
{
    return "2009, 2010";
}

QList<PluginAuthor> CrosshairsPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Cezar Mocan", "cezarmocan@gmail.com" )
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" );
}

QIcon CrosshairsPlugin::icon () const
{
    return QIcon( ":/icons/crosshairs.png" );
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

void CrosshairsPlugin::setSettings( const QHash<QString,QVariant> &settings )
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
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    const int width = 21;
    const int height = 21;

    if ( m_crosshairs.isNull() ) {
        painter->setRenderHint( QPainter::Antialiasing, true );
        m_crosshairs = QPixmap( QSize( width, height ) );
        m_crosshairs.fill( Qt::transparent );
        QPainter mapPainter( &m_crosshairs );
        m_svgobj->render( &mapPainter );
    }

    GeoDataCoordinates const focusPoint = viewport->focusPoint();
    GeoDataCoordinates const centerPoint = GeoDataCoordinates( viewport->centerLongitude(), viewport->centerLatitude() );
    if ( focusPoint == centerPoint ) {
        // Focus point is in the middle of the screen. Special casing this avoids jittering.
        int centerX = viewport->size().width() / 2;
        int centerY = viewport->size().height() / 2;
        painter->drawPixmap( QPoint ( centerX - width / 2, centerY - height / 2 ), m_crosshairs );
    } else {
        qreal centerX = 0.0;
        qreal centerY = 0.0;
        viewport->screenCoordinates( focusPoint, centerX, centerY );
        painter->drawPixmap( QPoint ( centerX - width / 2, centerY - height / 2 ), m_crosshairs );
    }

    return true;
}

}

Q_EXPORT_PLUGIN2( CrosshairsPlugin, Marble::CrosshairsPlugin )

#include "CrosshairsPlugin.moc"
