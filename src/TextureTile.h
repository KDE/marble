//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// Description: TextureTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//


#ifndef __MARBLE__TEXTURETILE_H
#define __MARBLE__TEXTURETILE_H


#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtCore/QString>


/**
 *@author Torsten Rahn
 *
 * This class contains a Tile with a texture in it.  It is used for
 * drawing the map.
 */

class TextureTile : public QObject {
    Q_OBJECT

 public:
    TextureTile();

    virtual ~TextureTile();

    void loadTile( int x, int y, int level, 
           const QString& theme, bool requestRepaint = true );

    const int  depth() const        { return m_depth; }

    const bool used() const         { return m_used; }
    void       setUsed( bool used ) { m_used = used; }

    QImage    *rawtile()            { return m_rawtile; }
 
    uchar  **jumpTable8;
    uint   **jumpTable32;

 signals:
    void downloadTile( QString );
    void tileUpdate();

 public slots:
    void slotLoadTile( const QString& path );

 protected:
    QImage  *m_rawtile;

    int      m_depth;

    bool     m_used;
};


#endif // __MARBLE__TEXTURETILE_H
