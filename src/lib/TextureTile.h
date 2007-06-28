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


class TextureTile : public QObject {
    Q_OBJECT

 public:
    TextureTile( int id );

    virtual ~TextureTile();

    void loadTile( int x, int y, int level, 
           const QString& theme, bool requestTileUpdate = true );

    const int  depth() const        { return m_depth; }

    const bool used() const         { return m_used; }
    void       setUsed( bool used ) { m_used = used; }

    const QImage&     rawtile()            { return m_rawtile; }
 
    uchar  **jumpTable8;
    uint   **jumpTable32;

 Q_SIGNALS:
    void downloadTile( const QString& relativeUrlString, int id );
    void tileUpdateDone();

 public Q_SLOTS:
    void slotReLoadTile( int x, int y, int level, const QString& theme );

 protected:
    int      m_id;

    QImage   m_rawtile;

    int      m_depth;
    bool     m_used;
};


#endif // __MARBLE__TEXTURETILE_H
