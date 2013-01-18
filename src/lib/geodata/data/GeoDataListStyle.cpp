//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataListStyle.h"
#include "GeoDataTypes.h"
#include "MarbleDirs.h"

namespace Marble
{

class GeoDataListStylePrivate
{
public:
    GeoDataListStylePrivate();

    GeoDataListStyle::ListItemType m_listItemType;
    QColor m_bgColor;

    QVector<GeoDataItemIcon*> m_vector;
};

GeoDataListStylePrivate::GeoDataListStylePrivate() :
    m_listItemType( GeoDataListStyle::Check ),
    m_bgColor( Qt::white )
{
}

GeoDataListStyle::GeoDataListStyle() :
    d( new GeoDataListStylePrivate )
{
}

GeoDataListStyle::GeoDataListStyle( const Marble::GeoDataListStyle &other ) :
    GeoDataObject( other ), d( new GeoDataListStylePrivate( *other.d ) )
{
}

GeoDataListStyle &GeoDataListStyle::operator=( const GeoDataListStyle &other )
{
    GeoDataObject::operator=(other);
    *d = *other.d;
    return *this;
}

GeoDataListStyle::~GeoDataListStyle()
{
    delete d;
}

const char *GeoDataListStyle::nodeType() const
{
    return GeoDataTypes::GeoDataListStyleType;
}

GeoDataListStyle::ListItemType GeoDataListStyle::listItemType() const
{
    return d->m_listItemType;
}

void GeoDataListStyle::setListItemType( const ListItemType &type )
{
    d->m_listItemType = type;
}

QColor GeoDataListStyle::backgroundColor() const
{
    return d->m_bgColor;
}

void GeoDataListStyle::setBackgroundColor( const QColor &color )
{
    d->m_bgColor = color;
}

QVector<GeoDataItemIcon*> GeoDataListStyle::itemIconList() const
{
    return d->m_vector;
}

GeoDataItemIcon* GeoDataListStyle::child( int i )
{
    return d->m_vector.at(i);
}

const GeoDataItemIcon* GeoDataListStyle::child( int i ) const
{
    return d->m_vector.at(i);
}

int GeoDataListStyle::childPosition( GeoDataItemIcon* object )
{
    return d->m_vector.indexOf( object );
}

void GeoDataListStyle::append( GeoDataItemIcon *other )
{
    other->setParent( this );
    d->m_vector.append( other );
}


void GeoDataListStyle::remove( int index )
{
    d->m_vector.remove( index );
}

int GeoDataListStyle::size() const
{
    return d->m_vector.size();
}

GeoDataItemIcon& GeoDataListStyle::at( int pos )
{
    return *(d->m_vector[ pos ]);
}

const GeoDataItemIcon& GeoDataListStyle::at( int pos ) const
{
    return *(d->m_vector.at( pos ));
}

GeoDataItemIcon& GeoDataListStyle::last()
{
    return *(d->m_vector.last());
}

const GeoDataItemIcon& GeoDataListStyle::last() const
{
    return *(d->m_vector.last());
}

GeoDataItemIcon& GeoDataListStyle::first()
{
    return *(d->m_vector.first());
}

const GeoDataItemIcon& GeoDataListStyle::first() const
{
    return *(d->m_vector.first());
}

void GeoDataListStyle::clear()
{
    qDeleteAll(d->m_vector);
    d->m_vector.clear();
}

QVector<GeoDataItemIcon*>::Iterator GeoDataListStyle::begin()
{
    return d->m_vector.begin();
}

QVector<GeoDataItemIcon*>::Iterator GeoDataListStyle::end()
{
    return d->m_vector.end();
}

QVector<GeoDataItemIcon*>::ConstIterator GeoDataListStyle::constBegin() const
{
    return d->m_vector.constBegin();
}

QVector<GeoDataItemIcon*>::ConstIterator GeoDataListStyle::constEnd() const
{
    return d->m_vector.constEnd();
}

void GeoDataListStyle::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );
    stream << d->m_vector.count();

    for ( QVector <GeoDataItemIcon*>::const_iterator iterator = d->m_vector.constBegin();
          iterator != d->m_vector.constEnd();
          ++iterator )
    {
        const GeoDataItemIcon *itemIcon = *iterator;
        itemIcon->pack( stream );
    }
}

void GeoDataListStyle::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    int count;
    stream >> count;

    int featureId;
    stream >> featureId;

    GeoDataItemIcon *itemIcon = new GeoDataItemIcon;
    itemIcon->unpack( stream );
    d->m_vector.append( itemIcon );
}

}
