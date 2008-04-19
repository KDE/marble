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

#include "SunLocator.h"
#include "MergedLayerDecorator.h"

class TextureTile : public QObject {
    Q_OBJECT

 public:
    TextureTile( int id );

    virtual ~TextureTile();
    
    void loadRawTile(const QString& theme, int level, int x, int y);

    int  id() const           { return m_id; }
    int  depth() const        { return m_depth; }

    bool used() const         { return m_used; }
    void setUsed( bool used ) { m_used = used; }

    int numBytes() const      { return m_rawtile.numBytes(); }

    const QImage& rawtile()   { return m_rawtile; }
    QImage* tile()            { return &m_rawtile; }
 
    uchar  **jumpTable8;
    uint   **jumpTable32;

 Q_SIGNALS:
    void tileUpdateDone();
    void downloadTile(const QString& relativeUrlString, const QString& id);

 public Q_SLOTS:
    void   loadTile( bool requestTileUpdate = true );

 protected:
    void     showTileId( QImage& worktile, QString theme, int level, int x, int y );

    int      m_id;

    QImage   m_rawtile;

    int      m_depth;
    bool     m_used;
};


#endif // __MARBLE__TEXTURETILE_H
