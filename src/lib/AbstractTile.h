//
// This file is part of the Marble Desktop Globe.
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


class QDateTime;

namespace Marble
{

class AbstractTilePrivate;
class TileId;

class AbstractTile : public QObject
{
    Q_OBJECT

 public:

    explicit AbstractTile( TileId const& tid, QObject * parent = 0 );
    virtual ~AbstractTile();

    enum TileState {
        TileEmpty,
        TilePartial,
        TileComplete
    };

    enum DatasetState {
        DatasetEmpty,
        DatasetZeroLevel,
        DatasetScaled,
        DatasetComplete
    };

 public:
    TileId const& id() const;

    bool used() const;
    void setUsed( bool used );

    TileState state() const;
    void setState( TileState state );

    const QDateTime & created() const;
    void setCreated( const QDateTime &created );

protected:
    AbstractTilePrivate * const d_ptr;
    AbstractTile(AbstractTilePrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE( AbstractTile )
    Q_DISABLE_COPY( AbstractTile )

};

}

#endif // MARBLE_ABSTRACTTILE_H
