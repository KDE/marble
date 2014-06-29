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
#include "PanoramioWidget.h"

// Marble
#include "MarbleDebug.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoSceneLayer.h"

using namespace Marble;

PanoramioWidget::PanoramioWidget( QObject *parent )
    : AbstractDataPluginItem( parent )
{
}

bool PanoramioWidget::initialized() const
{
    return !smallImage.isNull();
}
    
void PanoramioWidget::addDownloadedFile( const QString &url, const QString &type )
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
        mDebug() << "PanoramioWidget: addDownloadFile can't handle type " << type;
    }
}

QDate PanoramioWidget::uploadDate() const
{
    return m_uploadDate;
}
    
void PanoramioWidget::setUploadDate( QDate uploadDate )
{
    m_uploadDate = uploadDate;
}

bool PanoramioWidget::operator<( const AbstractDataPluginItem *other ) const
{
    Q_ASSERT( dynamic_cast<const PanoramioWidget *>( other ) != 0 );

    return uploadDate() > static_cast<const PanoramioWidget *>( other )->uploadDate();
}

void PanoramioWidget::paint( QPainter *painter )
{
    painter->drawImage( 0, 0, smallImage );
}

#include "PanoramioWidget.moc"
