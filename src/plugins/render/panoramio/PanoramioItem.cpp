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

#include <QPainter>

using namespace Marble;

PanoramioItem::PanoramioItem( QObject *parent )
    : AbstractDataPluginItem( parent )
{
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
        smallImage = largeImage.scaled( largeImage.size() / 2,
                                        Qt::IgnoreAspectRatio,
                                        Qt::SmoothTransformation );
        update();
    }
    else {
        mDebug() << Q_FUNC_INFO << "can't handle type" << type;
    }
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

#include "PanoramioItem.moc"
