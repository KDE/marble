//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>

#include "ui_OpenCachingCacheDialog.h"

namespace Marble
{

// That's the font we will use to paint.
QFont OpenCachingItem::s_font = QFont( "Sans Serif", 8 );
QPixmap OpenCachingItem::s_icon = QPixmap( "/usr/share/icons/oxygen/32x32/status/folder-open.png" );

OpenCachingItem::OpenCachingItem( const OpenCachingCache& cache, QObject *parent )
    : AbstractDataPluginItem( parent ), m_cache( cache ), m_infoDialog( 0), m_action( new QAction( this ) ), m_logIndex( 0 )
{
    // The size of an item without a text is 0
    setSize( QSize( m_cache.difficulty() * 10, m_cache.difficulty() * 10 ) );
    s_font.setBold( true );
    updateTooltip();
    setId( QString( "opencache-%1" ).arg( cache.id() ) );
    setCoordinate( GeoDataCoordinates( cache.longitude(), cache.latitude(), 0.0, GeoDataCoordinates::Degree ) );
    setTarget( "earth" );
    connect( m_action, SIGNAL(triggered()),
             this, SLOT(showInfoDialog()) );
}

OpenCachingItem::~OpenCachingItem()
{
    // nothing to do
}

QString OpenCachingItem::itemType() const
{
    // Our itemType:
    return "OpenCachingItem";
}

bool OpenCachingItem::initialized()
{
    return m_cache.id() != 0;
}

void OpenCachingItem::showInfoDialog()
{
    if( !m_infoDialog ) {
        m_infoDialog = infoDialog();
    }
    Q_ASSERT( m_infoDialog );
    m_infoDialog->show();
}

QDialog *OpenCachingItem::infoDialog()
{
    if ( !m_infoDialog ) {
        // Initializing information dialog
        m_infoDialog = new QDialog();
        m_ui = new Ui::OpenCachingCacheDialog;
        m_ui->setupUi( m_infoDialog );
        m_ui->m_cacheName->setText( m_cache.cacheName() );
        m_ui->m_country->setText( m_cache.country() );
        m_ui->m_dateCreated->setText( m_cache.dateCreated().toString( Qt::SystemLocaleShortDate ) );
        m_ui->m_dateHidden->setText( m_cache.dateHidden().toString( Qt::SystemLocaleShortDate ) );
        m_ui->m_dateLastModified->setText( m_cache.dateCreated().toString( Qt::SystemLocaleShortDate ) );
        m_ui->m_difficulty->setText( QString::number( m_cache.difficulty() ) );
        m_ui->m_latitude->setText( QString::number( m_cache.latitude() ) );
        m_ui->m_longitude->setText( QString::number( m_cache.longitude() ) );
        m_ui->m_size->setText( m_cache.sizeString() );
        m_ui->m_status->setText( m_cache.status() );
        m_ui->m_terrain->setText( QString::number( m_cache.terrain() ) );
        m_ui->m_type->setText( m_cache.cacheType() );
        m_ui->m_userName->setText( m_cache.userName() );
        QHash<QString, OpenCachingCacheDescription> descriptions = m_cache.description();
        QStringList languages = descriptions.keys();
        qSort( languages );
        m_ui->m_languageBox->addItems( languages );
        if( descriptions.size() > 0 ) {
            updateDescriptionLanguage( languages.first() );
            connect( m_ui->m_languageBox, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(updateDescriptionLanguage(QString)) );
        }
        OpenCachingCacheLog log = m_cache.log();
        if( log.size() > 0 ) {
            m_ui->m_logText->setHtml( log[m_logIndex].text() );
            m_ui->m_logCount->setText( "1 / " + QString::number( log.size() ) );
            connect( m_ui->m_nextButton, SIGNAL(clicked()),
                     this, SLOT(nextLogEntry()) );
            connect( m_ui->m_previousButton, SIGNAL(clicked()),
                     this, SLOT(previousLogEntry()) );
        }
        if( log.size() > 1 ) {
            m_ui->m_nextButton->setEnabled( true );
        }
        QPushButton *closeButton = m_ui->m_buttonBox->button( QDialogButtonBox::Close );
        connect( closeButton, SIGNAL(clicked()),
                 m_infoDialog, SLOT(close()) );
        m_infoDialog->setWindowTitle( m_cache.cacheName() );
    }
    return m_infoDialog;
}

QAction *OpenCachingItem::action()
{
    m_action->setText( m_cache.cacheName() );
    return m_action;
}

bool OpenCachingItem::operator<( const AbstractDataPluginItem *other ) const
{
    // FIXME shorter distance to current position?
    const OpenCachingItem* item = dynamic_cast<const OpenCachingItem*>( other );
    return item ? m_cache.id() < item->m_cache.id() : false;
}

void OpenCachingItem::paint( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    // Save the old painter state.
    painter->save();
    painter->autoMapQuality();

    qreal width;
    qreal height;

    if( !s_icon.isNull() ) {
        width = s_icon.width();
        height = s_icon.height();
        painter->drawPixmap( 0, 0, s_icon );
    }
    else {
        width = m_cache.difficulty() * 10;
        height = m_cache.difficulty() * 10;
        // Draws the circle with circles' center as rectangle's top-left corner.
        QRect arcRect( 0, 0, width, height );
        QColor color = Oxygen::brickRed4;
        if ( m_cache.difficulty() < 2.0 ) {
            color = Oxygen::sunYellow6;
        } else if ( m_cache.difficulty() < 4.0 ) {
            color = Oxygen::hotOrange4;
        }
        painter->setPen( QPen( Qt::NoPen ) );
        QBrush brush( color );
        brush.setColor( color );
        painter->setBrush( brush );
        painter->drawEllipse( arcRect );
    }

    // Draws difficulty of the cache
    QFontMetrics metrics( s_font );
    QString difficultyText = QString::number( m_cache.difficulty() );
    QRect difficultyRect = metrics.boundingRect( difficultyText );
    painter->setBrush( QBrush() );
    painter->setPen( QPen() );
    painter->setFont( s_font );
    painter->drawText( QPoint( (width - difficultyRect.width()) / 2, (height - difficultyRect.height()) / 2 + metrics.ascent() ), difficultyText );

    // Restore the old painter state.
    painter->restore();
}

void OpenCachingItem::updateTooltip()
{
    QString html = "<table cellpadding=\"2\">";
    if ( m_cache.id() != 0 ) {
        html += tr( "<tr><td align=\"right\">Cache name</td>" );
        html += "<td>" + m_cache.cacheName() + "</td></tr>";
        html += tr( "<tr><td align=\"right\">User name</td><td>" ) + m_cache.userName() + "</td></tr>";
        if ( !m_cache.cacheName().isEmpty() ) {
            html += tr( "<tr><td align=\"right\">Date hidden</td><td>" ) + m_cache.dateHidden().toString( Qt::SystemLocaleShortDate ) + "</td></tr>";
        }
        html += tr( "<tr><td align=\"right\">Difficulty</td><td>" ) + QString::number( m_cache.difficulty() ) + "</td></tr>";
        html += tr( "<tr><td align=\"right\">Size</td><td>" ) + m_cache.sizeString() + "</td></tr>";
        html += "</table>";
        setToolTip( html );
    }
}

void OpenCachingItem::updateDescriptionLanguage( QString language )
{
    QHash<QString, OpenCachingCacheDescription> descriptions = m_cache.description();
    QString text = descriptions[language].shortDescription() + "\n\n" +
            descriptions[language].description() + "\n\n" +
            descriptions[language].hint() + "\n\n";
    m_ui->m_descriptionText->setHtml( text );
}

void OpenCachingItem::nextLogEntry()
{
    OpenCachingCacheLog log = m_cache.log();
    if( m_logIndex + 1 < log.size() ) {
        m_logIndex++;
        m_ui->m_logText->setHtml( log[m_logIndex].text() );
        m_ui->m_logCount->setText( QString::number( m_logIndex + 1 ) + " / " + QString::number( log.size() ) );
        m_ui->m_previousButton->setEnabled( true );
        if( m_logIndex == log.size() - 1 ) {
            m_ui->m_nextButton->setEnabled( false );
        }
    }
}

void OpenCachingItem::previousLogEntry()
{
    OpenCachingCacheLog log = m_cache.log();
    if( m_logIndex - 1 >= 0 ) {
        m_logIndex--;
        m_ui->m_logText->setHtml( log[m_logIndex].text() );
        m_ui->m_logCount->setText( QString::number( m_logIndex + 1 ) + " / " + QString::number( log.size() ) );
        m_ui->m_nextButton->setEnabled( true );
        if( m_logIndex == 0 ) {
            m_ui->m_previousButton->setEnabled( false );
        }
    }
}

}

#include "OpenCachingItem.moc"
