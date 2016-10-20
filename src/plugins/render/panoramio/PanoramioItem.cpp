//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PanoramioItem.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleWidget.h"
#include "layers/PopupLayer.h"

#include <QAction>
#include <QPainter>

using namespace Marble;

PanoramioItem::PanoramioItem( MarbleWidget *marbleWidget, QObject *parent ) :
    AbstractDataPluginItem( parent ),
    m_marbleWidget( marbleWidget )
{
    m_action = new QAction( this );
    connect( m_action, SIGNAL(triggered()), this, SLOT(openBrowser()) );
}

bool PanoramioItem::initialized() const
{
    return !smallImage.isNull();
}

void PanoramioItem::addDownloadedFile( const QString &url, const QString &type )
{
    if( standardImageSize == type ) {
        // Loading original image
        QImage largeImage;
        largeImage.load( url );

        // Scaling the image to the half of the original size
        smallImage = largeImage.scaled( largeImage.size() / 3,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation );
        setSize( smallImage.size() );
        update();
    }
    else {
        mDebug() << Q_FUNC_INFO << "can't handle type" << type;
    }
}

void PanoramioItem::setPhotoUrl( const QUrl &url )
{
    m_url = url;
}

QDate PanoramioItem::uploadDate() const
{
    return m_uploadDate;
}

void PanoramioItem::setUploadDate( const QDate &uploadDate )
{
    m_uploadDate = uploadDate;
}

bool PanoramioItem::operator<( const AbstractDataPluginItem *other ) const
{
    Q_ASSERT( dynamic_cast<const PanoramioItem *>( other ) != 0 );

    return uploadDate() > static_cast<const PanoramioItem *>( other )->uploadDate();
}

void PanoramioItem::paint( QPainter *painter )
{
    painter->drawImage( 0, 0, smallImage );
}

QAction *Marble::PanoramioItem::action()
{
    if( m_action->icon().isNull() ) {
        m_action->setIcon( QIcon( QPixmap::fromImage( smallImage ) ) );
    }

    return m_action;
}

void PanoramioItem::openBrowser()
{
    if ( m_marbleWidget ) {
        PopupLayer* popup = m_marbleWidget->popupLayer();
        popup->setCoordinates( coordinate(), Qt::AlignRight | Qt::AlignVCenter );
        popup->setSize(QSizeF(720, 470));
        popup->setUrl( m_url );
        popup->popup();
    }
}

#include "moc_PanoramioItem.cpp"
