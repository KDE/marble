//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
//

#include "ImageLoadThread.h"

#include <QtGui/QColor>
#include "MarbleDebug.h"

#include "TileId.h"

using namespace Marble;

ImageLoadThread::ImageLoadThread( QObject *parent )
    : QThread( parent )
{
}

ImageLoadThread::~ImageLoadThread()
{
}

void ImageLoadThread::run()
{
}

QImage ImageLoadThread::take( const TileId &tileId )
{
    Q_UNUSED( tileId )

    QImage tile( 675, 675, QImage::Format_RGB32 );
    tile.fill( QColor( 0,0,255,255 ).rgb() );
    return tile;
} 

#include "ImageLoadThread.moc"
