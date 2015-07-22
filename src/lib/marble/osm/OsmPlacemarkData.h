//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMPLACEMARKDATA_H
#define MARBLE_OSMPLACEMARKDATA_H

// Qt
#include <QHash>
#include <QMetaType>
#include <QString>

// Marble
#include "GeoDataCoordinates.h"
#include <marble_export.h>

namespace Marble
{

class GeoDataGeometry;
class GeoDataPlacemark;
class OsmWayData;
class OsmRelationData;
class GeoWriter;

/**
 * This class is used to encapsulate the osm data fields kept within a placemark's extendedData.
 * It stores OSM server generated data: id, version, changeset, uid, visible, user, timestamp;
 * It also stores a hash map of <tags> ( key-value mappings ) and a hash map of component osm
 * placemarks @see m_ndRefs @see m_memberRefs
 *
 * The usual workflow with osmData goes as follows:
 *
 * Parsing stage:
 * The OsmParser parses tags (they have server-generated attributes), creates new placemarks and
 * assigns them new OsmPlacemarkData objects with all the needed information.
 *
 * Editing stage:
 * While editing placemarks that have OsmPlacemarkData, all relevant changes reflect on the
 * OsmPlacemarkData object as well, so as not to uncorrelate data from the actual placemarks.
 *
 * Writing stage:
 * The OsmObjectManager assigns OsmPlacemarkData objects to placemarks that do not have it
 * ( these are usually newly created placemarks within the editor, or placemarks loaded from
 * ".kml" files ). Placemarks that already have it, are simply written as-is.
 */
class MARBLE_EXPORT OsmPlacemarkData
{

public:
    OsmPlacemarkData();

    qint64 id() const;
    QString version() const;
    QString changeset() const;
    QString uid() const;
    QString isVisible() const;
    QString user() const;
    QString timestamp() const;
    QString action() const;

    void setId( qint64 id );
    void setVersion( const QString& version );
    void setChangeset( const QString& changeset );
    void setUid( const QString& uid );
    void setVisible( const QString& visible );
    void setUser( const QString& user );
    void setTimestamp( const QString& timestamp );
    void setAction( const QString& action );

    /**
     * @brief iterators for the tags hash.
     */
    QHash< QString, QString >::const_iterator tagsBegin() const;
    QHash< QString, QString >::const_iterator tagsEnd() const;

    /**
     * @brief iterators for the reference hashes.
     */
    QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator nodeReferencesBegin() const;
    QHash< GeoDataCoordinates, OsmPlacemarkData >::const_iterator nodeReferencesEnd() const;
    QHash< const GeoDataGeometry*, OsmPlacemarkData >::const_iterator memberReferencesBegin() const;
    QHash< const GeoDataGeometry*, OsmPlacemarkData >::const_iterator memberReferencesEnd() const;

    /**
     * @brief this function returns the osmData associated with a member
     */
    OsmPlacemarkData &reference( const GeoDataGeometry* geometry );
    OsmPlacemarkData reference( const GeoDataGeometry* geometry ) const;

    /**
     * @brief this function returns the osmData assosciated with a nd
     */
    OsmPlacemarkData &reference( const GeoDataCoordinates& coordinates );
    OsmPlacemarkData reference( const GeoDataCoordinates& coordinates ) const;

    /**
     * @brief addRef this function inserts a GeoDataCoordinates = OsmplacemarkData
     * mapping into the reference hash, equivalent to the osm <nd ref="@param key" >
     * core data element
     */
    void addReference( const GeoDataGeometry* key, const OsmPlacemarkData &value );

    /**
     * @brief addRef this function inserts a GeoDataGeometry=OsmplacemarkData
     * mapping into the reference hash, equivalent to the <member ref="@param key" >
     * osm core data element
     */
    void addReference( const GeoDataCoordinates& key, const OsmPlacemarkData &value );

    /**
     * @brief addTag this function inserts a string key=value mapping,
     * equivalent to the <tag k="@param key" v="@param value"> osm core data
     * element
     */
    void addTag( const QString& key, const QString& value );

    /**
     * @brief osmData is stored within a placemark's extended data hash
     * at an entry with osmKey
     */
    static QString osmHashKey();

private:
    qint64 m_id;
    QString m_version;
    QString m_changeset;
    QString m_uid;
    QString m_visible;
    QString m_user;
    QString m_timestamp;
    QString m_action;
    QHash<QString, QString> m_tags;
    static const QString osmDataKey;

    /**
     * @brief m_ndRefs is used to store a way's component nodes
     * ( It is empty for other placemark types )
     */
    QHash< GeoDataCoordinates, OsmPlacemarkData > m_nodeReferences;

    /**
     * @brief m_memberRefs is used to store a relations's members
     * ( It is empty for other placemark types )
     */
    QHash< const GeoDataGeometry*, OsmPlacemarkData > m_memberReferences;

};

}

// Makes qvariant_cast possible for OsmPlacemarkData objects
Q_DECLARE_METATYPE( Marble::OsmPlacemarkData )

#endif
