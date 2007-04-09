//
// C++ Interface: TextureTile
//
// Description: TextureTile contains a single image quadtile 
// and jumptables for faster access to the pixel data
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution


#ifndef __MARBLE__TEXTURETILE_H
#define __MARBLE__TEXTURETILE_H


#include <QtCore/QObject>
#include <QtCore/QHash>
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
    TextureTile( int x, int y, int level, const QString& theme );

    virtual ~TextureTile();

    const int  depth() const        { return m_depth; }

    const bool used() const         { return m_used; }
    void       setUsed( bool used ) { m_used = used; }

    QImage    *rawtile()            { return m_rawtile; }
 
    uchar  **jumpTable8;
    uint   **jumpTable32;

 signals:
    void downloadTile( const QString& filename );
    void tileUpdate();

 public slots:
    void slotLoadTile( const QString& path );

 protected:
    void loadTile( int x, int y, int level, 
		   const QString& theme, bool requestRepaint = true );

 protected:
    QImage  *m_rawtile;

    int      m_depth;

    bool     m_used;
};


#endif // __MARBLE__TEXTURETILE_H
