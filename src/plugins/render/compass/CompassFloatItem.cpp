//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
//

#include "CompassFloatItem.h"
#include "ui_CompassConfigWidget.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ViewportParams.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtSvg/QSvgRenderer>

namespace Marble
{

CompassFloatItem::CompassFloatItem()
    : AbstractFloatItem( 0 ),
      m_svgobj( 0 )
{
}

CompassFloatItem::CompassFloatItem( const MarbleModel *marbleModel )
    : AbstractFloatItem( marbleModel, QPointF( -1.0, 10.0 ), QSizeF( 75.0, 75.0 ) ),
      m_isInitialized( false ),
      m_svgobj( 0 ),
      m_compass(),
      m_polarity( 0 ),
      m_configDialog( 0 ),
      m_uiConfigWidget( 0 )
{
}

CompassFloatItem::~CompassFloatItem ()
{
    delete m_svgobj;
}

QStringList CompassFloatItem::backendTypes() const
{
    return QStringList( "compass" );
}

QString CompassFloatItem::name() const
{
    return tr( "Compass" );
}

QString CompassFloatItem::guiString() const
{
    return tr( "&Compass" );
}

QString CompassFloatItem::nameId() const
{
    return QString( "compass" );
}

QString CompassFloatItem::version() const
{
    return "1.0";
}

QString CompassFloatItem::description() const
{
    return tr( "This is a float item that provides a compass." );
}

QString CompassFloatItem::copyrightYears() const
{
    return "2009, 2010";
}

QList<PluginAuthor> CompassFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" );
}

QIcon CompassFloatItem::icon() const
{
    return QIcon(":/icons/compass.png");
}

void CompassFloatItem::initialize()
{
    readSettings();
    m_isInitialized = true;
}

bool CompassFloatItem::isInitialized() const
{
    return m_isInitialized;
}

QPainterPath CompassFloatItem::backgroundShape() const
{
    QRectF contentRect = this->contentRect();
    QPainterPath path;
    int fontheight = QFontMetrics( font() ).ascent();
    int compassLength = static_cast<int>( contentRect.height() ) - 5 - fontheight;

    path.addEllipse( QRectF( QPointF( marginLeft() + padding() + ( contentRect.width() - compassLength ) / 2,
                                      marginTop() + padding() + 5 + fontheight ),
                             QSize( compassLength, compassLength ) ).toRect() );
    return path;
}

void CompassFloatItem::changeViewport( ViewportParams *viewport )
{
    // figure out the polarity ...
    if ( m_polarity != viewport->polarity() ) {
        m_polarity = viewport->polarity();
        update();
    }
}

void CompassFloatItem::paintContent( QPainter *painter )
{
    painter->save();

    QRectF compassRect( contentRect() );

    QString dirstr = tr( "N" );
    if ( m_polarity == -1 )
        dirstr = tr( "S" );
    if ( m_polarity == 0 )
        dirstr = "";

    int fontheight = QFontMetrics( font() ).ascent();
    int fontwidth = QFontMetrics( font() ).boundingRect( dirstr ).width();

    QPen outlinepen( background().color() );
    outlinepen.setWidth( 2 );
    QBrush outlinebrush( pen().color() );

    QPainterPath   outlinepath;
    const QPointF  baseline( 0.5 * (qreal)( compassRect.width() - fontwidth ),
                             (qreal)(fontheight) + 2.0 );

    outlinepath.addText( baseline, font(), dirstr );

    painter->setPen( outlinepen );
    painter->setBrush( outlinebrush );
    painter->drawPath( outlinepath );

    painter->setPen( Qt::NoPen );
    painter->drawPath( outlinepath );

    int compassLength = static_cast<int>( compassRect.height() ) - 5 - fontheight;
        
    QSize compassSize( compassLength, compassLength ); 

    // Rerender compass pixmap if the size has changed
    if ( m_compass.isNull() || m_compass.size() != compassSize ) {
        m_compass = QPixmap( compassSize );
        m_compass.fill( Qt::transparent );
        QPainter mapPainter( &m_compass );
        mapPainter.setViewport( m_compass.rect() );
        m_svgobj->render( &mapPainter ); 
    }
    painter->drawPixmap( QPoint( static_cast<int>( compassRect.width() - compassLength ) / 2, fontheight + 5 ), m_compass );

    painter->restore();
}

QDialog *CompassFloatItem::configDialog()
{
    if ( !m_configDialog ) {
        m_configDialog = new QDialog();
        m_uiConfigWidget = new Ui::CompassConfigWidget;
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

QHash<QString,QVariant> CompassFloatItem::settings() const
{
    QHash<QString, QVariant> result = AbstractFloatItem::settings();

    foreach ( const QString &key, m_settings.keys() ) {
        result.insert( key, m_settings[key] );
    }

    return result;
}

void CompassFloatItem::setSettings( const QHash<QString,QVariant> &settings )
{
    AbstractFloatItem::setSettings( settings );

    m_settings = settings;

    readSettings();
}

void CompassFloatItem::readSettings()
{
    int index = m_settings.value( "theme", 0 ).toInt();
    if ( m_uiConfigWidget && index >= 0 && index < m_uiConfigWidget->m_themeList->count() ) {
        m_uiConfigWidget->m_themeList->setCurrentRow( index );
    }

    QString theme = ":/compass.svg";
    switch( index ) {
    case 1:
        theme = ":/compass-arrows.svg";
        break;
    case 2:
        theme = ":/compass-atom.svg";
        break;
    case 3:
        theme = ":/compass-magnet.svg";
        break;
    }

    delete m_svgobj;
    CompassFloatItem * me = const_cast<CompassFloatItem*>( this );
    m_svgobj = new QSvgRenderer( theme, me );
    m_compass = QPixmap();
}

void CompassFloatItem::writeSettings()
{
    if ( m_uiConfigWidget ) {
        m_settings["theme"] = m_uiConfigWidget->m_themeList->currentRow();
    }
    readSettings();
    update();
    emit settingsChanged( nameId() );
}

}

Q_EXPORT_PLUGIN2( CompassFloatItem, Marble::CompassFloatItem )

#include "CompassFloatItem.moc"
