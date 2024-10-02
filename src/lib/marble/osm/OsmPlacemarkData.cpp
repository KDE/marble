// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

// Self
#include "osm/OsmPlacemarkData.h"

// Marble
#include "GeoDataExtendedData.h"

#include <QXmlStreamAttributes>

namespace Marble
{

inline uint qHash(Marble::OsmIdentifier ident, uint seed)
{
    return ::qHash(ident.id, seed) ^ ::qHash((int)ident.type, seed);
}

OsmPlacemarkData::OsmPlacemarkData()
    : m_id(0)
    , m_href(std::make_shared<OsmPlacemarkDataHashRef>())
{
    // nothing to do
}

OsmPlacemarkData::~OsmPlacemarkData() = default;

OsmPlacemarkDataHashRef *OsmPlacemarkData::hRef() const
{
    return m_href.get();
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
    return m_tags.value(QStringLiteral("mx:changeset"));
}

QString OsmPlacemarkData::version() const
{
    return m_tags.value(QStringLiteral("mx:version"));
}

QString OsmPlacemarkData::uid() const
{
    return m_tags.value(QStringLiteral("mx:uid"));
}

QString OsmPlacemarkData::isVisible() const
{
    return m_tags.value(QStringLiteral("mx:visible"));
}

QString OsmPlacemarkData::user() const
{
    return m_tags.value(QStringLiteral("mx:user"));
}

QString OsmPlacemarkData::timestamp() const
{
    return m_tags.value(QStringLiteral("mx:timestamp"));
}

QString OsmPlacemarkData::action() const
{
    return m_tags.value(QStringLiteral("mx:action"));
}

void OsmPlacemarkData::setId(qint64 id)
{
    m_id = id;
}

void OsmPlacemarkData::setVersion(const QString &version)
{
    m_tags[QStringLiteral("mx:version")] = version;
}

void OsmPlacemarkData::setChangeset(const QString &changeset)
{
    m_tags[QStringLiteral("mx:changeset")] = changeset;
}

void OsmPlacemarkData::setUid(const QString &uid)
{
    m_tags[QStringLiteral("mx:uid")] = uid;
}

void OsmPlacemarkData::setVisible(const QString &visible)
{
    m_tags[QStringLiteral("mx:visible")] = visible;
}

void OsmPlacemarkData::setUser(const QString &user)
{
    m_tags[QStringLiteral("mx:user")] = user;
}

void OsmPlacemarkData::setTimestamp(const QString &timestamp)
{
    m_tags[QStringLiteral("mx:timestamp")] = timestamp;
}

void OsmPlacemarkData::setAction(const QString &action)
{
    m_tags[QStringLiteral("mx:action")] = action;
}

QString OsmPlacemarkData::tagValue(const QString &key) const
{
    return m_tags.value(key);
}

void OsmPlacemarkData::addTag(const QString &key, const QString &value)
{
    m_tags.insert(key, value);
}

void OsmPlacemarkData::removeTag(const QString &key)
{
    m_tags.remove(key);
}

bool OsmPlacemarkData::containsTag(const QString &key, const QString &value) const
{
    auto const iter = m_tags.constFind(key);
    return iter == m_tags.constEnd() ? false : iter.value() == value;
}

bool OsmPlacemarkData::containsTagKey(const QString &key) const
{
    return m_tags.contains(key);
}

QHash<QString, QString>::const_iterator OsmPlacemarkData::findTag(const QString &key) const
{
    return m_tags.constFind(key);
}

QHash<QString, QString>::const_iterator OsmPlacemarkData::tagsBegin() const
{
    return m_tags.begin();
}

QHash<QString, QString>::const_iterator OsmPlacemarkData::tagsEnd() const
{
    return m_tags.constEnd();
}

OsmPlacemarkData &OsmPlacemarkData::nodeReference(const GeoDataCoordinates &coordinates)
{
    return m_href->m_nodeReferences[coordinates];
}

OsmPlacemarkData OsmPlacemarkData::nodeReference(const GeoDataCoordinates &coordinates) const
{
    return m_href->m_nodeReferences.value(coordinates);
}

void OsmPlacemarkData::addNodeReference(const GeoDataCoordinates &key, const OsmPlacemarkData &value)
{
    m_href->m_nodeReferences.insert(key, value);
}

void OsmPlacemarkData::removeNodeReference(const GeoDataCoordinates &key)
{
    m_href->m_nodeReferences.remove(key);
}

bool OsmPlacemarkData::containsNodeReference(const GeoDataCoordinates &key) const
{
    return m_href->m_nodeReferences.contains(key);
}

void OsmPlacemarkData::changeNodeReference(const GeoDataCoordinates &oldKey, const GeoDataCoordinates &newKey)
{
    m_href->m_nodeReferences.insert(newKey, m_href->m_nodeReferences.value(oldKey));
    m_href->m_nodeReferences.remove(oldKey);
}

OsmPlacemarkData &OsmPlacemarkData::memberReference(int key)
{
    return m_href->m_memberReferences[key];
}

OsmPlacemarkData OsmPlacemarkData::memberReference(int key) const
{
    return m_href->m_memberReferences.value(key);
}

void OsmPlacemarkData::addMemberReference(int key, const OsmPlacemarkData &value)
{
    m_href->m_memberReferences.insert(key, value);
}

void OsmPlacemarkData::removeMemberReference(int key)
{
    // If an inner boundary is deleted, all indexes higher than the deleted one
    // must be lowered by 1 to keep order.
    QHash<int, OsmPlacemarkData> newHash;
    QHash<int, OsmPlacemarkData>::iterator it = m_href->m_memberReferences.begin();
    QHash<int, OsmPlacemarkData>::iterator end = m_href->m_memberReferences.end();

    for (; it != end; ++it) {
        if (it.key() > key) {
            newHash.insert(it.key() - 1, it.value());
        } else if (it.key() < key) {
            newHash.insert(it.key(), it.value());
        }
    }
    m_href->m_memberReferences = newHash;
}

bool OsmPlacemarkData::containsMemberReference(int key) const
{
    return m_href->m_memberReferences.contains(key);
}

void OsmPlacemarkData::addRelation(qint64 id, OsmType type, const QString &role)
{
    m_relationReferences.insert({id, type}, role);
}

void OsmPlacemarkData::removeRelation(qint64 id)
{
    /// ### this is wrong and just done this way for backward behavior compatible
    /// ### this method should probably take type as an additional argument
    m_relationReferences.remove({id, OsmType::Node});
    m_relationReferences.remove({id, OsmType::Way});
    m_relationReferences.remove({id, OsmType::Relation});
}

bool OsmPlacemarkData::containsRelation(qint64 id) const
{
    /// ### this is wrong and just done this way for backward behavior compatible
    /// ### this method should probably take type as an additional argument
    return m_relationReferences.contains({id, OsmType::Node}) || m_relationReferences.contains({id, OsmType::Way})
        || m_relationReferences.contains({id, OsmType::Relation});
}

QHash<OsmIdentifier, QString>::const_iterator OsmPlacemarkData::relationReferencesBegin() const
{
    return m_relationReferences.begin();
}

QHash<OsmIdentifier, QString>::const_iterator OsmPlacemarkData::relationReferencesEnd() const
{
    return m_relationReferences.constEnd();
}

bool OsmPlacemarkData::isNull() const
{
    return !m_id;
}

bool OsmPlacemarkData::isEmpty() const
{
    return m_tags.isEmpty() && m_href->m_nodeReferences.isEmpty() && m_href->m_memberReferences.isEmpty() && m_relationReferences.isEmpty();
}

OsmPlacemarkData OsmPlacemarkData::fromParserAttributes(const QXmlStreamAttributes &attributes)
{
    OsmPlacemarkData osmData;
    osmData.setId(attributes.value(QLatin1StringView("id")).toLongLong());
    if (const auto s = attributes.value(QLatin1StringView("version")); !s.isEmpty()) {
        osmData.setVersion(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("changeset")); !s.isEmpty()) {
        osmData.setChangeset(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("user")); !s.isEmpty()) {
        osmData.setUser(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("uid")); !s.isEmpty()) {
        osmData.setUid(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("visible")); !s.isEmpty()) {
        osmData.setVisible(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("timestamp")); !s.isEmpty()) {
        osmData.setTimestamp(s.toString());
    }
    if (const auto s = attributes.value(QLatin1StringView("action")); !s.isEmpty()) {
        osmData.setAction(s.toString());
    }
    return osmData;
}

const char *OsmPlacemarkData::nodeType() const
{
    return "OsmPlacemarkDataType";
}

// ---------------------------------------------------------------------------------------------------------

OsmPlacemarkDataHashRef::OsmPlacemarkDataHashRef() = default;

QHash<GeoDataCoordinates, OsmPlacemarkData> &OsmPlacemarkDataHashRef::nodeReferences()
{
    return m_nodeReferences;
}

QHash<GeoDataCoordinates, OsmPlacemarkData>::const_iterator OsmPlacemarkDataHashRef::nodeReferencesBegin() const
{
    return m_nodeReferences.begin();
}

QHash<GeoDataCoordinates, OsmPlacemarkData>::const_iterator OsmPlacemarkDataHashRef::nodeReferencesEnd() const
{
    return m_nodeReferences.constEnd();
}

QHash<int, OsmPlacemarkData> &OsmPlacemarkDataHashRef::memberReferences()
{
    return m_memberReferences;
}

QHash<int, OsmPlacemarkData>::const_iterator OsmPlacemarkDataHashRef::memberReferencesBegin() const
{
    return m_memberReferences.begin();
}

QHash<int, OsmPlacemarkData>::const_iterator OsmPlacemarkDataHashRef::memberReferencesEnd() const
{
    return m_memberReferences.constEnd();
}

}
