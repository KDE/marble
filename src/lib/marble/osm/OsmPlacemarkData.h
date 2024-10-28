// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMPLACEMARKDATA_H
#define MARBLE_OSMPLACEMARKDATA_H

// Qt
#include <QHash>
#include <QMetaType>
#include <QString>

// Marble
#include "GeoDataCoordinates.h"
#include "GeoDocument.h"
#include <marble_export.h>

class QXmlStreamAttributes;

namespace Marble
{

/** Type of OSM element. */
enum class OsmType {
    Node,
    Way,
    Relation
};

/** Identifier for an OSM element.
 *  @note OSM uses distinct id spaces for all its three basic element types, so just the numeric id
 *  on its own doesn't identify an element without knowing its type.
 */
struct OsmIdentifier {
    inline OsmIdentifier() = default;
    inline OsmIdentifier(qint64 _id, OsmType _type)
        : id(_id)
        , type(_type)
    {
    }

    qint64 id = 0;
    OsmType type = OsmType::Way;

    inline bool operator==(OsmIdentifier other) const
    {
        return id == other.id && type == other.type;
    }
};

/** Forward declaration */
class OsmPlacemarkDataHashRef;

/**
 * This class is used to encapsulate the osm data fields kept within a placemark's extendedData.
 * It stores OSM server generated data: id, version, changeset, uid, visible, user, timestamp;
 * It also stores a hash map of \<tags\> ( key-value mappings ) and a hash map of component osm
 * placemarks @see m_nodeReferences @see m_memberReferences
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
class MARBLE_EXPORT OsmPlacemarkData : public GeoNode
{
public:
    OsmPlacemarkData();
    ~OsmPlacemarkData() override;

    qint64 id() const;
    qint64 oid() const;
    QString version() const;
    QString changeset() const;
    QString uid() const;
    QString isVisible() const;
    QString user() const;
    QString timestamp() const;
    QString action() const;
    const char *nodeType() const override;

    void setId(qint64 id);
    void setVersion(const QString &version);
    void setChangeset(const QString &changeset);
    void setUid(const QString &uid);
    void setVisible(const QString &visible);
    void setUser(const QString &user);
    void setTimestamp(const QString &timestamp);
    void setAction(const QString &action);

    /**
     * @brief tagValue returns the value of the tag that has @p key as key
     * or an empty qstring if there is no such tag
     */
    QString tagValue(const QString &key) const;

    /**
     * @brief addTag this function inserts a string key=value mapping,
     * equivalent to the \<tag k="@p key" v="@p value"\> osm core data
     * element
     */
    void addTag(const QString &key, const QString &value);

    /**
     * @brief removeTag removes the tag from the tag hash
     */
    void removeTag(const QString &key);

    /**
     * @brief containsTag returns true if the tag hash contains an entry with
     * the @p key as key and @p value as value
     */
    bool containsTag(const QString &key, const QString &value) const;

    /**
     * @brief containsTagKey returns true if the tag hash contains an entry with
     * the @p key as key
     */
    bool containsTagKey(const QString &key) const;

    /**
     * @brief tagValue returns a pointer to the tag that has @p key as key
     * or the end iterator if there is no such tag
     */
    QHash<QString, QString>::const_iterator findTag(const QString &key) const;

    /**
     * @brief iterators for the tags hash.
     */
    QHash<QString, QString>::const_iterator tagsBegin() const;
    QHash<QString, QString>::const_iterator tagsEnd() const;

    /**
     * @brief this function returns the osmData associated with a nd
     */
    OsmPlacemarkData &nodeReference(const GeoDataCoordinates &coordinates);
    OsmPlacemarkData nodeReference(const GeoDataCoordinates &coordinates) const;

    /**
     * @brief addRef this function inserts a GeoDataCoordinates = OsmPlacemarkData
     * mapping into the reference hash, equivalent to the \<member ref="@p key" \>
     * osm core data element
     */
    void addNodeReference(const GeoDataCoordinates &key, const OsmPlacemarkData &value);
    void removeNodeReference(const GeoDataCoordinates &key);
    bool containsNodeReference(const GeoDataCoordinates &key) const;

    /**
     * @brief changeNodeReference is a convenience function that allows the quick change of
     * a node hash entry. This is generally used to update the osm data in case
     * nodes are being moved in the editor.
     */
    void changeNodeReference(const GeoDataCoordinates &oldKey, const GeoDataCoordinates &newKey);

    /**
     * @brief this function returns the osmData associated with a member boundary's index
     * -1 represents the outer boundary of a polygon, and 0,1,2... the inner boundaries,
     * in the order provided by polygon->innerBoundaries();
     */
    OsmPlacemarkData &memberReference(int key);
    OsmPlacemarkData memberReference(int key) const;

    /**
     * @brief addRef this function inserts a int = OsmplacemarkData
     * mapping into the reference hash, equivalent to the osm \<nd ref="@p boundary of index @key" \>
     * core data element
     * @see m_memberReferences
     */
    void addMemberReference(int key, const OsmPlacemarkData &value);
    void removeMemberReference(int key);
    bool containsMemberReference(int key) const;

    /**
     * @brief addRelation calling this makes the osm placemark a member of the relation
     * with @p id as id, while having the role @p role
     */
    void addRelation(qint64 id, OsmType type, const QString &role);
    void removeRelation(qint64 id);
    bool containsRelation(qint64 id) const;

    QHash<OsmIdentifier, QString>::const_iterator relationReferencesBegin() const;
    QHash<OsmIdentifier, QString>::const_iterator relationReferencesEnd() const;

    /**
     * @brief isNull returns false if the osmData is loaded from a source
     * or true if its just default constructed
     */
    bool isNull() const;

    /**
     * @brief isEmpty returns true if no attribute other than the id has been set
     */
    bool isEmpty() const;

    /**
     * @brief fromParserAttributes is a convenience function that parses all osm-related
     * arguments of a tag
     * @return an OsmPlacemarkData object containing all the necessary data
     */
    static OsmPlacemarkData fromParserAttributes(const QXmlStreamAttributes &attributes);

    /**
     * Return the insternal instance of the hash-table functions container.
     */
    OsmPlacemarkDataHashRef *hRef() const;

private:
    qint64 m_id;
    QHash<QString, QString> m_tags;

    /**
     * @brief m_relationReferences is used to store the relations the placemark is part of
     * and the role it has within them.
     * Eg. an entry ( "123", "stop" ) means that the parent placemark is a member of
     * the relation with id "123", while having the "stop" role
     */
    QHash<OsmIdentifier, QString> m_relationReferences;

    /**
     * Store the insternal instance of the hash-table functions container.
     */
    std::shared_ptr<OsmPlacemarkDataHashRef> m_href;
};

/**
 * Container to host hash-table functions with OsmPlacemarkData as values.
 * This container is necessary with Qt 6.6 under MSVC 2022 as compiler refuse to build
 * a QHash of a not fully defined class as value.
 *
 * E:\dk\x64-windows\include\Qt6\QtCore/qhash.h(76,7): error C2079: 'QHashPrivate::Node<Key,T>::value' uses undefined class 'Marble::OsmPlacemarkData'
 * [C:\Users\gilles\Documents\marble\build.vcpkg\src\lib\marble\marblewidget.vcxproj] with
 *         [
 *             Key=Marble::GeoDataCoordinates,
 *             T=Marble::OsmPlacemarkData
 *         ]
 * E:\dk\x64-windows\include\Qt6\QtCore/qhash.h(858,1): message : see reference to class template instantiation 'QHashPrivate::Node<Key,T>' being compiled
 * [C:\Users\gilles\Documents\marble\build.vcpkg\src\lib\marble\marblewidget.vcxproj] with
 *         [
 *             Key=Marble::GeoDataCoordinates,
 *             T=Marble::OsmPlacemarkData
 *         ]
 *
 */
class MARBLE_EXPORT OsmPlacemarkDataHashRef
{
public:
    OsmPlacemarkDataHashRef();

    /**
     * @brief iterators for the reference hashes.
     */
    QHash<GeoDataCoordinates, OsmPlacemarkData> &nodeReferences();
    QHash<GeoDataCoordinates, OsmPlacemarkData>::const_iterator nodeReferencesBegin() const;
    QHash<GeoDataCoordinates, OsmPlacemarkData>::const_iterator nodeReferencesEnd() const;

    QHash<int, OsmPlacemarkData> &memberReferences();
    QHash<int, OsmPlacemarkData>::const_iterator memberReferencesBegin() const;
    QHash<int, OsmPlacemarkData>::const_iterator memberReferencesEnd() const;

    /**
     * @brief m_ndRefs is used to store a way's component nodes
     * ( It is empty for other placemark types )
     */
    QHash<GeoDataCoordinates, OsmPlacemarkData> m_nodeReferences;

    /**
     * @brief m_memberRefs is used to store a polygon's member boundaries
     *  the key represents the index of the boundary within the polygon geometry:
     *  -1 represents the outerBoundary, and 0,1,2... its innerBoundaries, in the
     *  order provided by polygon->innerBoundaries()
     */
    QHash<int, OsmPlacemarkData> m_memberReferences;
};

}

// Makes qvariant_cast possible for OsmPlacemarkData objects
Q_DECLARE_METATYPE(Marble::OsmPlacemarkData)

#endif
