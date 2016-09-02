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

#include <QRect>
#include <QColor>
#include <QPushButton>
#include <QSvgRenderer>
#include <QImageReader>


namespace Marble
{

CrosshairsPlugin::CrosshairsPlugin()
    : RenderPlugin( 0 ),
      m_svgobj( 0 ),
      m_themeIndex( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
}

CrosshairsPlugin::CrosshairsPlugin( const MarbleModel *marbleModel )
    : RenderPlugin( marbleModel ),
      m_isInitialized( false ),
      m_svgobj( 0 ),
      m_themeIndex( 0 ),
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
    return QStringList(QStringLiteral("crosshairs"));
}

QString CrosshairsPlugin::renderPolicy() const
{
    return QStringLiteral("ALWAYS");
}

QStringList CrosshairsPlugin::renderPosition() const
{
    return QStringList(QStringLiteral("FLOAT_ITEM")); // although this is not a float item we choose the position of one
}

RenderPlugin::RenderType CrosshairsPlugin::renderType() const
{
    return RenderPlugin::TopLevelRenderType;
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
    return QStringLiteral("crosshairs");
}

QString CrosshairsPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString CrosshairsPlugin::description() const
{
    return tr( "A plugin that shows crosshairs." );
}

QString CrosshairsPlugin::copyrightYears() const
{
    return QStringLiteral("2009, 2010");
}

QVector<PluginAuthor> CrosshairsPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Cezar Mocan"), QStringLiteral("cezarmocan@gmail.com"))
            << PluginAuthor(QStringLiteral("Torsten Rahn"), QStringLiteral("tackat@kde.org"));
}

QIcon CrosshairsPlugin::icon () const
{
    return QIcon(QStringLiteral(":/icons/crosshairs.png"));
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
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL(accepted()),
                SLOT(writeSettings()) );
        connect( m_uiConfigWidget->m_buttonBox, SIGNAL(rejected()),
                SLOT(readSettings()) );
        QPushButton *applyButton = m_uiConfigWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 this,        SLOT(writeSettings()) );
    }

    return m_configDialog;
}

QHash<QString,QVariant> CrosshairsPlugin::settings() const
{
    QHash<QString, QVariant> result = RenderPlugin::settings();

    result.insert(QStringLiteral("theme"), m_themeIndex);

    return result;
}

void CrosshairsPlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    RenderPlugin::setSettings( settings );

    m_themeIndex = settings.value(QStringLiteral("theme"), 0).toInt();

    readSettings();
}


void CrosshairsPlugin::readSettings()
{
    if ( m_uiConfigWidget && m_themeIndex >= 0 && m_themeIndex < m_uiConfigWidget->m_themeList->count() ) {
        m_uiConfigWidget->m_themeList->setCurrentRow( m_themeIndex );
    }

    m_theme = QStringLiteral(":/crosshairs-darkened.png");
    switch( m_themeIndex ) {
    case 1:
        m_theme = QStringLiteral(":/crosshairs-gun1.svg");
        break;
    case 2:
        m_theme = QStringLiteral(":/crosshairs-gun2.svg");
        break;
    case 3:
        m_theme = QStringLiteral(":/crosshairs-circled.svg");
        break;
    case 4:
        m_theme = QStringLiteral(":/crosshairs-german.svg");
        break;
    }

    if (QImageReader::imageFormat(m_theme) == QLatin1String("svg")) {
        delete m_svgobj;
        m_svgobj = new QSvgRenderer( m_theme, this );
    }
    m_crosshairs = QPixmap();
}

void CrosshairsPlugin::writeSettings()
{
    if ( m_uiConfigWidget ) {
        m_themeIndex = m_uiConfigWidget->m_themeList->currentRow();
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

    if ( m_crosshairs.isNull() ) {
        if (QImageReader::imageFormat(m_theme) == QLatin1String("svg")) {
            painter->setRenderHint( QPainter::Antialiasing, true );
            m_crosshairs = QPixmap( QSize( 21, 21 ) );
            m_crosshairs.fill( Qt::transparent );

            QPainter mapPainter( &m_crosshairs );
            m_svgobj->render( &mapPainter );
        }
        else {
            m_crosshairs.load( m_theme );
        }
    }

    const int width = m_crosshairs.width();
    const int height = m_crosshairs.height();

    int posX;
    int posY;

    GeoDataCoordinates const focusPoint = viewport->focusPoint();
    GeoDataCoordinates const centerPoint = GeoDataCoordinates( viewport->centerLongitude(), viewport->centerLatitude() );
    if ( focusPoint == centerPoint ) {
        // Focus point is in the middle of the screen. Special casing this avoids jittering.
        const QSize viewPortSize = viewport->size();
        posX = (viewPortSize.width() - width) / 2;
        posY = (viewPortSize.height() - height) / 2;
    } else {
        qreal centerX = 0.0;
        qreal centerY = 0.0;
        viewport->screenCoordinates( focusPoint, centerX, centerY );
        posX = qRound(centerX - width / 2.0);
        posY = qRound(centerY - height / 2.0);
    }
    painter->drawPixmap(posX, posY, m_crosshairs );

    return true;
}

}

#include "moc_CrosshairsPlugin.cpp"
