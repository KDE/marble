//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <tackat@kde.org>
//

//
// Description: ImageLoadThread loads images for tiles in the background
//


#ifndef MARBLE_IMAGELOADTHREAD_H
#define MARBLE_IMAGELOADTHREAD_H

#include <QtCore/QThread>
#include <QtGui/QImage>

namespace Marble
{

class TileId;

class ImageLoadThread : public QThread
{
    Q_OBJECT

 public:
    ImageLoadThread( QObject *parent = 0 );
    ~ImageLoadThread();

    QImage take( const TileId &tileId ); 

    void run();
};

}

#endif // MARBLE_IMAGELOADTHREAD_H
