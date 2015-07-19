//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

// Self
#include "osm/OsmPlacemarkData.h"

// Qt
#include <QVariant>
#include <QFile>

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataExtendedData.h"
#include "GeoWriter.h"

namespace Marble
{

const QString OsmPlacemarkData::osmDataKey = "osm_data";

OsmPlacemarkData::OsmPlacemarkData():
    m_id( 0 )
{
    // nothing to do
}

qint64 OsmPlacemarkData::id() const
{
    return m_id;
}

QString OsmPlacemarkData::changeset() const
{
    return m_changeset;
}

QString OsmPlacemarkData::version() const
{
    return m_version;
}

QString OsmPlacemarkData::uid() const
{
    return m_uid;
}

QString OsmPlacemarkData::isVisible() const
{
    return m_visible;
}

QString OsmPlacemarkData::user() const
{
    return m_user;
}

QString OsmPlacemarkData::timestamp() const
{
    return m_timestamp;
}

QString OsmPlacemarkData::action() const
{
    return m_action;
}

QHash< QString, QString >::const_iterator OsmPlacemarkData::tagsBegin() const
{
    return m_tags.begin();
}

QHash< QString, QString >::const_iterator OsmPlacemarkData::tagsEnd() const
{
    return m_tags.constEnd();
}

void OsmPlacemarkData::setId( qint64 id )
{
    m_id = id;
}

void OsmPlacemarkData::setVersion( const QString& version )
{
    m_version = version;
}

void OsmPlacemarkData::setChangeset( const QString& changeset )
{
    m_changeset = changeset;
}

void OsmPlacemarkData::setUid( const QString& uid )
{
    m_uid = uid;
}

void OsmPlacemarkData::setVisible( const QString& visible )
{
    m_visible = visible;
}

void OsmPlacemarkData::setUser( const QString& user )
{
    m_user = user;
}

void OsmPlacemarkData::setTimestamp( const QString& timestamp )
{
    m_timestamp = timestamp;
}

void OsmPlacemarkData::setAction( const QString& action )
{
    m_action = action;
}

void OsmPlacemarkData::addTag( const QString& key, const QString& value )
{
    m_tags.insert( key, value );
}

OsmPlacemarkData &OsmPlacemarkData::reference( const GeoDataCoordinates &coordinates )
{
    return m_nodeReferences[ coordinates ];
}

OsmPlacemarkData OsmPlacemarkData::reference( const GeoDataCoordinates &coordinates ) const
{
    return m_nodeReferences.value( coordinates );
}

OsmPlacemarkData &OsmPlacemarkData::reference( const GeoDataGeometry *geometry )
{
    return m_memberReferences[ geometry ];
}

OsmPlacemarkData OsmPlacemarkData::reference( const GeoDataGeometry *geometry ) const
{
    return m_memberReferences.value( geometry );
}

void OsmPlacemarkData::addReference( const GeoDataCoordinates &key, const OsmPlacemarkData &value )
{
    m_nodeReferences.insert( key, value );
}

void OsmPlacemarkData::addReference( const GeoDataGeometry* key, const OsmPlacemarkData &value )
{
    m_memberReferences.insert( key, value );
}

QHash< const GeoDataGeometry*, OsmPlacemarkData >::const_iterator OsmPlacemarkData::memberReferencesBegin() const
{
    return m_memberReferences.begin();
}

QHash< const GeoDataGeometry*, OsmPlacemarkData >::const_iterator OsmPlacemarkData::memberReferencesEnd() const
{
    return m_memberReferences.constEnd();
}

QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator OsmPlacemarkData::nodeReferencesBegin() const
{
    return m_nodeReferences.begin();
}

QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator OsmPlacemarkData::nodeReferencesEnd() const
{
    return m_nodeReferences.constEnd();
}

QString OsmPlacemarkData::osmHashKey()
{
    return osmDataKey;
}

}
