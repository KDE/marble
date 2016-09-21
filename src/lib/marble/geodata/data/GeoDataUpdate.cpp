//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataUpdate.h"

#include "GeoDataCreate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "GeoDataTypes.h"

#include <QString>

namespace Marble
{

class GeoDataUpdatePrivate
{
public:
    GeoDataUpdatePrivate();
    GeoDataCreate* m_create;
    GeoDataDelete* m_delete;
    GeoDataChange* m_change;
    QString m_targetHref;
};

GeoDataUpdatePrivate::GeoDataUpdatePrivate() :
    m_create( 0 ), m_delete( 0 ), m_change( 0 )
{
}

GeoDataUpdate::GeoDataUpdate() :
    d( new GeoDataUpdatePrivate )
{
}

GeoDataUpdate::GeoDataUpdate( const Marble::GeoDataUpdate &other ) :
    GeoDataObject(), d( new GeoDataUpdatePrivate( *other.d ) )
{
}

GeoDataUpdate &GeoDataUpdate::operator=( const GeoDataUpdate &other )
{
    GeoDataObject::operator =( other );
    *d = *other.d;
    return *this;
}

bool GeoDataUpdate::operator==(const GeoDataUpdate& other) const
{
    bool const changeEmpty = !d->m_change || d->m_change->size() == 0;
    bool const otherChangeEmpty = !other.d->m_change || other.d->m_change->size() == 0;

    if( changeEmpty != otherChangeEmpty ) {
        return false;
    } else if( d->m_change && other.d->m_change && *d->m_change != *other.d->m_change ) {
        return false;
    }

    return d->m_targetHref == other.d->m_targetHref;
}

bool GeoDataUpdate::operator!=(const GeoDataUpdate& other) const
{
    return !this->operator==(other);
}

GeoDataUpdate::~GeoDataUpdate()
{
    delete d;
}

const char *GeoDataUpdate::nodeType() const
{
    return GeoDataTypes::GeoDataUpdateType;
}

QString GeoDataUpdate::targetHref() const
{
    return d->m_targetHref;
}

void GeoDataUpdate::setTargetHref( const QString &targetHref )
{
    d->m_targetHref = targetHref;
}

GeoDataChange* GeoDataUpdate::change() const
{
    return d->m_change;
}

void GeoDataUpdate::setChange( GeoDataChange* change )
{
    delete d->m_change;
    d->m_change = change;
    if ( d->m_change ) {
        d->m_change->setParent( this );
    }
}

GeoDataCreate* GeoDataUpdate::create() const
{
    return d->m_create;
}

void GeoDataUpdate::setCreate( GeoDataCreate* create )
{
    delete d->m_create;
    d->m_create = create;
    if ( d->m_create ) {
        d->m_create->setParent( this );
    }
}

GeoDataDelete* GeoDataUpdate::getDelete() const
{
    return d->m_delete;
}

void GeoDataUpdate::setDelete( GeoDataDelete* dataDelete )
{
    delete d->m_delete;
    d->m_delete = dataDelete;
    if ( d->m_delete ) {
        d->m_delete->setParent( this );
    }
}

}
