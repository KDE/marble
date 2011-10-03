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
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QPushButton>
#include <QtSvg/QSvgRenderer>

namespace Marble
{

CompassFloatItem::CompassFloatItem ( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_isInitialized( false ),
      m_aboutDialog(0),
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

QString CompassFloatItem::description() const
{
    return tr( "This is a float item that provides a compass." );
}

QIcon CompassFloatItem::icon() const
{
    return QIcon(":/icons/compass.png");
}

QDialog* CompassFloatItem::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Compass Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author tackat, earthwings;
        
        earthwings.name = QString::fromUtf8( "Dennis Nienhüser" );
        earthwings.task = tr( "Developer" );
        earthwings.email = "earthwings@gentoo.org";
        authors.append( earthwings );
        
        tackat.name = "Torsten Rahn";
        tackat.task = tr( "Developer" );
        tackat.email = "tackat@kde.org";
        authors.append( tackat );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
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
        update();
    }
}

void CompassFloatItem::paintContent( GeoPainter *painter,
                                     ViewportParams *viewport,
                                     const QString& renderPos,
                                     GeoSceneLayer * layer )
{
    Q_UNUSED( layer )
    Q_UNUSED( renderPos )

    painter->save();

    painter->setRenderHint( QPainter::Antialiasing, true );

    QRectF compassRect( contentRect() );
    m_polarity = viewport->polarity();

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

    painter->autoMapQuality();

    int compassLength = static_cast<int>( compassRect.height() ) - 5 - fontheight;
        
    QSize compassSize( compassLength, compassLength ); 

    // Rerender compass pixmap if the size has changed
    if ( m_compass.isNull() || m_compass.size() != compassSize ) {
        m_compass = QPixmap( compassSize );
        m_compass.fill( Qt::transparent );
        QPainter mapPainter( &m_compass );
        mapPainter.setViewport( m_compass.rect() );
        m_svgobj->render( &mapPainter ); 
        mapPainter.setViewport( QRect( QPoint( 0, 0 ), viewport->size() ) );
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

QHash<QString,QVariant> CompassFloatItem::settings() const
{
    return m_settings;
}

void CompassFloatItem::setSettings( QHash<QString,QVariant> settings )
{
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
