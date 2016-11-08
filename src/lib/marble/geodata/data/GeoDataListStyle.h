//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#ifndef GEODATALISTSTYLE_H
#define GEODATALISTSTYLE_H

#include <QVector>

#include "MarbleGlobal.h"
#include "GeoDataObject.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataListStylePrivate;
class GeoDataItemIcon;

/**
 */
class GEODATA_EXPORT GeoDataListStyle : public GeoDataObject
{
public:
    GeoDataListStyle();

    GeoDataListStyle( const GeoDataListStyle &other );

    GeoDataListStyle& operator=( const GeoDataListStyle &other );

    bool operator==( const GeoDataListStyle &other ) const;
    bool operator!=( const GeoDataListStyle &other ) const;

    ~GeoDataListStyle();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    enum ListItemType {
        Check,
        RadioFolder,
        CheckOffOnly,
        CheckHideChildren
    };

    ListItemType listItemType() const;
    void setListItemType(ListItemType type);

    QColor backgroundColor() const;
    void setBackgroundColor( const QColor &color );

    QVector<GeoDataItemIcon*> itemIconList() const;

    GeoDataItemIcon* child( int );

    const GeoDataItemIcon* child( int ) const;

    int childPosition( const GeoDataItemIcon *child ) const;

    void append( GeoDataItemIcon *other );

    void remove( int index );

    int size() const;

    GeoDataItemIcon& at( int pos );
    const GeoDataItemIcon& at( int pos ) const;

    GeoDataItemIcon& last();
    const GeoDataItemIcon& last() const;

    GeoDataItemIcon& first();
    const GeoDataItemIcon& first() const;

    QVector<GeoDataItemIcon*>::Iterator begin();
    QVector<GeoDataItemIcon*>::Iterator end();
    QVector<GeoDataItemIcon*>::ConstIterator constBegin() const;
    QVector<GeoDataItemIcon*>::ConstIterator constEnd() const;
    void clear();

    virtual void pack( QDataStream& stream ) const;

    virtual void unpack( QDataStream& stream );

private:
    friend class GeoDataItemIcon;
    GeoDataListStylePrivate* const d;
};

}

#endif
