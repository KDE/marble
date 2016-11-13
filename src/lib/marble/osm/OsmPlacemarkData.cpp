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

// Marble
#include "GeoDataExtendedData.h"

#include <QXmlStreamAttributes>

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

qint64 OsmPlacemarkData::oid() const
{
    auto const value = m_tags.value(QStringLiteral("mx:oid")).toLong();
    return value > 0 ? value : m_id;
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



QString OsmPlacemarkData::tagValue( const QString& key ) const
{
    return m_tags.value( key );
}

void OsmPlacemarkData::addTag( const QString& key, const QString& value )
{
    m_tags.insert( key, value );
}

void OsmPlacemarkData::removeTag( const QString &key )
{
    m_tags.remove( key );
}

bool OsmPlacemarkData::containsTag( const QString &key, const QString &value ) const
{
    auto const iter = m_tags.constFind(key);
    return iter == m_tags.constEnd() ? false : iter.value() == value;
}

bool OsmPlacemarkData::containsTagKey( const QString &key ) const
{
    return m_tags.contains( key );
}

QHash<QString, QString>::const_iterator OsmPlacemarkData::findTag(const QString &key) const
{
    return m_tags.constFind(key);
}

QHash< QString, QString >::const_iterator OsmPlacemarkData::tagsBegin() const
{
    return m_tags.begin();
}

QHash< QString, QString >::const_iterator OsmPlacemarkData::tagsEnd() const
{
    return m_tags.constEnd();
}





OsmPlacemarkData &OsmPlacemarkData::nodeReference( const GeoDataCoordinates &coordinates )
{
    return m_nodeReferences[ coordinates ];
}

OsmPlacemarkData OsmPlacemarkData::nodeReference( const GeoDataCoordinates &coordinates ) const
{
    return m_nodeReferences.value( coordinates );
}

void OsmPlacemarkData::addNodeReference( const GeoDataCoordinates &key, const OsmPlacemarkData &value )
{
    m_nodeReferences.insert( key, value );
}

void OsmPlacemarkData::removeNodeReference( const GeoDataCoordinates &key )
{
    m_nodeReferences.remove( key );
}

bool OsmPlacemarkData::containsNodeReference( const GeoDataCoordinates &key ) const
{
    return m_nodeReferences.contains( key );
}

void OsmPlacemarkData::changeNodeReference( const GeoDataCoordinates &oldKey, const GeoDataCoordinates &newKey )
{
    m_nodeReferences.insert( newKey, m_nodeReferences.value( oldKey ) );
    m_nodeReferences.remove( oldKey );
}

QHash<GeoDataCoordinates, OsmPlacemarkData> &OsmPlacemarkData::nodeReferences()
{
    return m_nodeReferences;
}

QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator OsmPlacemarkData::nodeReferencesBegin() const
{
    return m_nodeReferences.begin();
}

QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator OsmPlacemarkData::nodeReferencesEnd() const
{
    return m_nodeReferences.constEnd();
}


OsmPlacemarkData &OsmPlacemarkData::memberReference( int key )
{
    return m_memberReferences[ key ];
}

OsmPlacemarkData OsmPlacemarkData::memberReference( int key ) const
{
    return m_memberReferences.value( key );
}


void OsmPlacemarkData::addMemberReference( int key, const OsmPlacemarkData &value )
{
    m_memberReferences.insert( key, value );
}

void OsmPlacemarkData::removeMemberReference( int key )
{
    // If an inner boundary is deleted, all indexes higher than the deleted one
    // must be lowered by 1 to keep order.
    QHash< int, OsmPlacemarkData > newHash;
    QHash< int, OsmPlacemarkData >::iterator it = m_memberReferences.begin();
    QHash< int, OsmPlacemarkData >::iterator end = m_memberReferences.end();

    for ( ; it != end; ++it ) {
        if ( it.key() > key ) {
            newHash.insert( it.key() - 1, it.value() );
        }
        else if ( it.key() < key ) {
            newHash.insert( it.key(), it.value() );
        }
    }
    m_memberReferences = newHash;
}

bool OsmPlacemarkData::containsMemberReference( int key ) const
{
    return m_memberReferences.contains( key );
}

QHash<int, OsmPlacemarkData> &OsmPlacemarkData::memberReferences()
{
    return m_memberReferences;
}

QHash< int, OsmPlacemarkData >::const_iterator OsmPlacemarkData::memberReferencesBegin() const
{
    return m_memberReferences.begin();
}

QHash< int, OsmPlacemarkData >::const_iterator OsmPlacemarkData::memberReferencesEnd() const
{
    return m_memberReferences.constEnd();
}

void OsmPlacemarkData::addRelation( qint64 id, const QString &role )
{
    m_relationReferences.insert( id, role );
}

void OsmPlacemarkData::removeRelation( qint64 id )
{
    m_relationReferences.remove( id );
}

bool OsmPlacemarkData::containsRelation( qint64 id ) const
{
    return m_relationReferences.contains( id );
}

QHash< qint64, QString >::const_iterator OsmPlacemarkData::relationReferencesBegin() const
{
    return m_relationReferences.begin();
}

QHash< qint64, QString >::const_iterator OsmPlacemarkData::relationReferencesEnd() const
{
    return m_relationReferences.constEnd();
}

QString OsmPlacemarkData::osmHashKey()
{
    return osmDataKey;
}

bool OsmPlacemarkData::isNull() const
{
    return !m_id;
}

bool OsmPlacemarkData::isEmpty() const
{
    return m_tags.isEmpty() &&
            m_nodeReferences.isEmpty() &&
            m_memberReferences.isEmpty() &&
            m_relationReferences.isEmpty() &&
            m_version.isEmpty() &&
            m_changeset.isEmpty() &&
            m_uid.isEmpty() &&
            m_user.isEmpty() &&
            m_timestamp.isEmpty();
}

OsmPlacemarkData OsmPlacemarkData::fromParserAttributes( const QXmlStreamAttributes &attributes )
{
    OsmPlacemarkData osmData;
    osmData.setId(attributes.value(QLatin1String("id")).toLongLong());
    osmData.setVersion(attributes.value(QLatin1String("version")).toString());
    osmData.setChangeset(attributes.value(QLatin1String("changeset")).toString());
    osmData.setUser(attributes.value(QLatin1String("user")).toString());
    osmData.setUid(attributes.value(QLatin1String("uid")).toString());
    osmData.setVisible(attributes.value(QLatin1String("visible")).toString());
    osmData.setTimestamp(attributes.value(QLatin1String("timestamp")).toString());
    osmData.setAction(attributes.value(QLatin1String("action")).toString());
    return osmData;
}

const char *OsmPlacemarkData::nodeType() const
{
    return "OsmPlacemarkDataType";
}

}
