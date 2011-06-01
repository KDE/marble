//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoGraphicsScene.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoGraphicsItem.h"

namespace Marble
{

bool zValueLessThan( GeoGraphicsItem* i1, GeoGraphicsItem* i2 )
{
    return i1->zValue() < i2->zValue();
}

class GeoGraphicsScenePrivate
{
public:
    QList< GeoGraphicsItem* > m_items;
};

GeoGraphicsScene::GeoGraphicsScene( QObject* parent ): QObject( parent ), d( new GeoGraphicsScenePrivate() )
{

}

GeoGraphicsScene::~GeoGraphicsScene()
{
    delete d;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items() const
{
    return d->m_items;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items( const Marble::GeoDataLatLonAltBox& box ) const
{
    QList< GeoGraphicsItem* > result;
    for ( QList< GeoGraphicsItem* >::const_iterator i = d->m_items.constBegin(); i != d->m_items.constEnd(); i++ )
    {
        if ( box.intersects(( *i )->latLonAltBox() ) )
            result.append( *i );
    }
    return result;
}

void GeoGraphicsScene::removeItem( GeoGraphicsItem* item )
{
    d->m_items.removeOne( item );
}

void GeoGraphicsScene::clear()
{
    d->m_items.clear();
}

void GeoGraphicsScene::addIdem( GeoGraphicsItem* item )
{
    QList< GeoGraphicsItem* >::iterator position = qLowerBound( d->m_items.begin(), d->m_items.end(), item, zValueLessThan );
    d->m_items.insert( position, item );
}
};

#include "GeoGraphicsScene.moc"
