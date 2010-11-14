//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009    Torsten Rahn <tackat@kde.org>
//

//
// Description: AbstractTile contains the base class for a single quadtile 
//


#ifndef MARBLE_ABSTRACTTILE_H
#define MARBLE_ABSTRACTTILE_H

#include <QtCore/QObject>

namespace Marble
{

class AbstractTilePrivate;
class TileId;

/*!
    \class AbstractTile
    \brief A class that provides the basic properties of tiles .

    A tile is a chunk of data that describes the themed look of a certain 
    (geographic) area for a given zoom level. Each tile can be identified via
    a unique TileId.
    
    Depending on its type the tile can contain texture data, vector data 
    or a set of even more tile data. 
    
    Usually the tiles are organized in so called quad tiles: This means that
    with increasing zoom level four other tiles cover the same area as a 
    single "parent" tile in the previous zoom level. These four tiles have 
    the same pixel/file size as the "parent" tile.
    
    Via AbstractTile::used() it's possible to find out whether the tile is 
    being displayed on the screen.
*/

class AbstractTile : public QObject
{
    Q_OBJECT

 public:

    explicit AbstractTile( TileId const& tid, QObject * parent = 0 );
    virtual ~AbstractTile();

 public:
/*!
    \brief Returns a unique ID for the tile.
    \return A TileId object that encodes zoom level, position and map theme.
*/     
    TileId const& id() const;

/*!
    \brief Returns whether the tile is being used on the screen.
    \return Describes whether the tile is in active use by the texture mapping.
*/     
    bool used() const;
    void setUsed( bool used );

protected:
    AbstractTilePrivate * const d_ptr;
    AbstractTile(AbstractTilePrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE( AbstractTile )
    Q_DISABLE_COPY( AbstractTile )
};

}

#endif // MARBLE_ABSTRACTTILE_H
