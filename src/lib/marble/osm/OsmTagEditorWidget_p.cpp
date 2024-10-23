// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmTagEditorWidget_p.h"
#include "OsmTagEditorWidget.h"

// Marble
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataGeometry.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataStyle.h"
#include "OsmPlacemarkData.h"
#include "StyleBuilder.h"

// Qt
#include <QObject>
#include <QTreeWidget>

namespace Marble
{

const QString OsmTagEditorWidgetPrivate::m_customTagAdderText = QObject::tr("Add custom tag...");
OsmTagEditorWidgetPrivate::OsmTagEditorWidgetPrivate()
{
    // nothing to do
}

OsmTagEditorWidgetPrivate::~OsmTagEditorWidgetPrivate()
{
    // nothing to do
}

void OsmTagEditorWidgetPrivate::populateCurrentTagsList()
{
    // Name tag
    if (!m_placemark->name().isEmpty()) {
        QStringList itemText;

        // "name" is a standard OSM tag, don't translate
        itemText << QStringLiteral("name") << m_placemark->name();
        auto nameTag = new QTreeWidgetItem(itemText);
        nameTag->setDisabled(true);
        m_currentTagsList->addTopLevelItem(nameTag);
    }

    // Multipolygon type tag
    if (geodata_cast<GeoDataPolygon>(m_placemark->geometry())) {
        QStringList itemText;
        // "type" is a standard OSM tag, don't translate
        itemText << QStringLiteral("type") << QStringLiteral("multipolygon");
        auto typeTag = new QTreeWidgetItem(itemText);
        typeTag->setDisabled(true);
        m_currentTagsList->addTopLevelItem(typeTag);
    }

    // Other tags
    if (m_placemark->hasOsmData()) {
        const OsmPlacemarkData &osmData = m_placemark->osmData();
        QHash<QString, QString>::const_iterator it = osmData.tagsBegin();
        QHash<QString, QString>::const_iterator end = osmData.tagsEnd();
        for (; it != end; ++it) {
            QTreeWidgetItem *tagItem = tagWidgetItem(OsmTag(it.key(), it.value()));
            m_currentTagsList->addTopLevelItem(tagItem);
        }
    }

    // Custom tag adder item
    auto adderItem = new QTreeWidgetItem();
    adderItem->setText(0, m_customTagAdderText);
    adderItem->setForeground(0, Qt::gray);
    adderItem->setIcon(0, QIcon(QStringLiteral(":marble/list-add.png")));
    adderItem->setFlags(adderItem->flags() | Qt::ItemIsEditable);
    m_currentTagsList->addTopLevelItem(adderItem);
    m_currentTagsList->resizeColumnToContents(0);
    m_currentTagsList->resizeColumnToContents(1);
}

void OsmTagEditorWidgetPrivate::populatePresetTagsList()
{
    QList<OsmTag> tags = recommendedTags();
    for (const OsmTag &tag : std::as_const(tags)) {
        QTreeWidgetItem *tagItem = tagWidgetItem(tag);
        m_recommendedTagsList->addTopLevelItem(tagItem);
    }
}

QTreeWidgetItem *OsmTagEditorWidgetPrivate::tagWidgetItem(const OsmTag &tag)
{
    QStringList itemText;

    itemText << tag.first;
    itemText << (tag.second.isEmpty() ? QLatin1Char('<') + QObject::tr("value") + QLatin1Char('>') : tag.second);

    auto tagItem = new QTreeWidgetItem(itemText);

    return tagItem;
}

QList<OsmTagEditorWidgetPrivate::OsmTag> OsmTagEditorWidgetPrivate::recommendedTags() const
{
    static const QList<OsmTag> additionalOsmTags = createAdditionalOsmTags();

    QList<OsmTag> recommendedTags;

    QStringList filter = generateTagFilter();

    auto const osmTagMapping = StyleBuilder::osmTagMapping();
    for (auto iter = osmTagMapping.begin(), end = osmTagMapping.end(); iter != end; ++iter) {
        if (filter.contains(iter.key().first)) {
            recommendedTags += iter.key();
        }
    }

    for (const auto &additionalOsmTag : additionalOsmTags) {
        if (filter.contains(additionalOsmTag.first)) {
            recommendedTags += additionalOsmTag;
        }
    }

    return recommendedTags;
}

QStringList OsmTagEditorWidgetPrivate::generateTagFilter() const
{
    // TO DO: implement more dynamic criteria for the filter
    // based on https://taginfo.openstreetmap.org/ and https://wiki.openstreetmap.org/wiki/

    // Contains all keys that should pass through the filter ( eg. { "amenity"), QStringLiteral("landuse", etc.. } )
    QStringList filter;

    QStringList tags, tagsAux;
    OsmPlacemarkData osmData;
    if (m_placemark->hasOsmData()) {
        osmData = m_placemark->osmData();
    } else {
        osmData = OsmPlacemarkData();
    }

    // Patterns in order of usefulness

    // If the placemark is a node, and it doesn't already have any node-specific tags, recommend all node-specific tags
    tags = QStringList() << QStringLiteral("amenity=*") << QStringLiteral("shop=*") << QStringLiteral("transport=*") << QStringLiteral("tourism=*")
                         << QStringLiteral("historic=*") << QStringLiteral("power=*") << QStringLiteral("barrier=*");
    if (geodata_cast<GeoDataPoint>(m_placemark->geometry()) && !containsAny(osmData, tags)) {
        addPattern(filter, osmData, tags);
    }

    // If the placemark is a way, and it doesn't already have any way-specific tags, recommend all way-specific tags
    tags = QStringList() << QStringLiteral("highway=*") << QStringLiteral("waterway=*") << QStringLiteral("railway=*");
    if (geodata_cast<GeoDataLineString>(m_placemark->geometry()) && !containsAny(osmData, tags)) {
        addPattern(filter, osmData, tags);
    }

    // If the placemark is a polygon, and it doesn't already have any polygon-specific tags, recommend all polygon-specific tags
    tags = QStringList() << QStringLiteral("landuse=*") << QStringLiteral("leisure=*");
    if (geodata_cast<GeoDataPolygon>(m_placemark->geometry()) && !containsAny(osmData, tags)) {
        addPattern(filter, osmData, tags);
    }

    // If the placemark is a relation, recommend type=*
    tags = QStringList() << QStringLiteral("type=*");
    if (m_placemark->extendedData().value(QStringLiteral("osmRelation")).value().toString() == QLatin1StringView("yes")) {
        addPattern(filter, osmData, tags);
    }

    // If the placemark has type=route, recommend route=*, network=*, ref=*, operator=*
    tags = QStringList() << QStringLiteral("type=route");
    tagsAux = QStringList() << QStringLiteral("route=*") << QStringLiteral("network=*") << QStringLiteral("ref=*") << QStringLiteral("operator=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has type=route_master, recommend route_master=*,
    tags = QStringList() << QStringLiteral("type=route_master");
    tagsAux = QStringList() << QStringLiteral("route_master=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has type=public_transport, recommend public_transport=*,
    tags = QStringList() << QStringLiteral("type=public_transport");
    tagsAux = QStringList() << QStringLiteral("public_transport=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has type=waterway, recommend waterway=*,
    tags = QStringList() << QStringLiteral("type=waterway");
    tagsAux = QStringList() << QStringLiteral("waterway=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has type=enforcement, recommend enforcement=*,
    tags = QStringList() << QStringLiteral("type=enforcement");
    tagsAux = QStringList() << QStringLiteral("enforcement=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has amenity=place_of_worship, recommend religion=*
    tags = QStringList() << QStringLiteral("amenity=place_of_worship");
    tagsAux = QStringList() << QStringLiteral("religion=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has amenity=toilets, recommend drinking_water=*, indoor=*
    tags = QStringList() << QStringLiteral("amenity=toilets");
    tagsAux = QStringList() << QStringLiteral("drinking_water=*") << QStringLiteral("indoor=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has  tourism=hostel, tourism=hotel or tourism=motel, recommend rooms=*, beds=*, wheelchair=*
    tags = QStringList() << QStringLiteral("tourism=hotel") << QStringLiteral("tourism=hostel") << QStringLiteral("tourism=motel");
    tagsAux = QStringList() << QStringLiteral("rooms=*") << QStringLiteral("beds=*") << QStringLiteral("wheelchair=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has  tourism=*, shop=*, amenity=*, recommend website=*, email=*, fee=*
    tags = QStringList() << QStringLiteral("tourism=*") << QStringLiteral("shop=*") << QStringLiteral("amenity=*");
    tagsAux = QStringList() << QStringLiteral("website=*") << QStringLiteral("email=*") << QStringLiteral("fee=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has amenity=* shop=*, recommend building=*
    tags = QStringList() << QStringLiteral("amenity=*") << QStringLiteral("shop=*");
    tagsAux = QStringList() << QStringLiteral("building=*");
    if (containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark has highway=*, recommend "lanes=*"), QStringLiteral("maxspeed=*"), QStringLiteral("oneway=*"), QStringLiteral("service=*"),
    // QStringLiteral("bridge=*"), QStringLiteral("tunnel=*"
    tags = QStringList() << QStringLiteral("highway=*");
    tagsAux = QStringList() << QStringLiteral("lanes=*") << QStringLiteral("maxspeed=*") << QStringLiteral("maxheight=*") << QStringLiteral("maxweight=*")
                            << QStringLiteral("abutters=*") << QStringLiteral("oneway=*") << QStringLiteral("service=*") << QStringLiteral("bridge=*")
                            << QStringLiteral("tunnel=*");
    if (geodata_cast<GeoDataLineString>(m_placemark->geometry()) && containsAny(osmData, tags)) {
        addPattern(filter, osmData, tagsAux);
    }

    // If the placemark is a polygon, recommend "surface=*"
    tags = QStringList() << QStringLiteral("surface=*");
    if (geodata_cast<GeoDataPolygon>(m_placemark->geometry())) {
        addPattern(filter, osmData, tags);
    }

    // Always recommend these:
    tags = QStringList() << QStringLiteral("addr:street=*") << QStringLiteral("addr:housenumber=*") << QStringLiteral("addr:postcode=*")
                         << QStringLiteral("addr:country=*") << QStringLiteral("access=*");
    addPattern(filter, osmData, tags);

    return filter;
}

bool OsmTagEditorWidgetPrivate::containsAny(const OsmPlacemarkData &osmData, const QStringList &tags)
{
    for (const QString &tag : tags) {
        const QStringList tagSplit = tag.split(QLatin1Char('='));

        // Only "key=value" mappings should be checked
        Q_ASSERT(tagSplit.size() == 2);

        QString key = tagSplit.at(0);
        QString value = tagSplit.at(1);

        if (value == QLatin1StringView("*") && osmData.containsTagKey(key)) {
            return true;
        } else if (value != QLatin1StringView("*") && osmData.containsTag(key, value)) {
            return true;
        }
    }
    return false;
}

void OsmTagEditorWidgetPrivate::addPattern(QStringList &filter, const OsmPlacemarkData &osmData, const QStringList &tags)
{
    for (const QString &tag : tags) {
        const QStringList tagSplit = tag.split(QLatin1Char('='));
        QString key = tagSplit.at(0);
        if (!osmData.containsTagKey(key)) {
            filter << key;
        }
    }
}

QList<OsmTagEditorWidgetPrivate::OsmTag> OsmTagEditorWidgetPrivate::createAdditionalOsmTags()
{
    const QList<OsmTag> additionalOsmTags = QList<OsmTag>()

        // Recommended for nodes
        << OsmTag(QStringLiteral("power"), QStringLiteral("pole")) << OsmTag(QStringLiteral("power"), QStringLiteral("generator"))
        << OsmTag(QStringLiteral("barrier"), QStringLiteral("fence")) << OsmTag(QStringLiteral("barrier"), QStringLiteral("wall"))
        << OsmTag(QStringLiteral("barrier"), QStringLiteral("gate"))

        // Recommended for ways
        << OsmTag(QStringLiteral("lanes"), QStringLiteral("")) << OsmTag(QStringLiteral("maxspeed"), QStringLiteral(""))
        << OsmTag(QStringLiteral("maxheight"), QStringLiteral("")) << OsmTag(QStringLiteral("maxweight"), QStringLiteral(""))
        << OsmTag(QStringLiteral("oneway"), QStringLiteral("yes")) << OsmTag(QStringLiteral("service"), QStringLiteral("driveway"))
        << OsmTag(QStringLiteral("service"), QStringLiteral("parking_aisle")) << OsmTag(QStringLiteral("service"), QStringLiteral("alley"))
        << OsmTag(QStringLiteral("tunnel"), QStringLiteral("yes")) << OsmTag(QStringLiteral("abutters"), QStringLiteral("commercial"))
        << OsmTag(QStringLiteral("abutters"), QStringLiteral("industrial")) << OsmTag(QStringLiteral("abutters"), QStringLiteral("mixed"))
        << OsmTag(QStringLiteral("abutters"), QStringLiteral("residential"))

        // Recommended for areas
        << OsmTag(QStringLiteral("surface"), QStringLiteral("unpaved")) << OsmTag(QStringLiteral("surface"), QStringLiteral("paved"))
        << OsmTag(QStringLiteral("surface"), QStringLiteral("gravel")) << OsmTag(QStringLiteral("surface"), QStringLiteral("dirt"))
        << OsmTag(QStringLiteral("surface"), QStringLiteral("grass"))

        // Relations
        << OsmTag(QStringLiteral("type"), QStringLiteral("route")) << OsmTag(QStringLiteral("type"), QStringLiteral("route_master"))
        << OsmTag(QStringLiteral("type"), QStringLiteral("public_transport")) << OsmTag(QStringLiteral("type"), QStringLiteral("destination_sign"))
        << OsmTag(QStringLiteral("type"), QStringLiteral("waterway"))
        << OsmTag(QStringLiteral("type"), QStringLiteral("enforcement"))

        // Relations: route
        << OsmTag(QStringLiteral("route"), QStringLiteral("road")) << OsmTag(QStringLiteral("route"), QStringLiteral("bicycle"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("foot")) << OsmTag(QStringLiteral("route"), QStringLiteral("hiking"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("bus")) << OsmTag(QStringLiteral("route"), QStringLiteral("trolleybus"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("ferry")) << OsmTag(QStringLiteral("route"), QStringLiteral("detour"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("train")) << OsmTag(QStringLiteral("route"), QStringLiteral("tram"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("mtb")) << OsmTag(QStringLiteral("route"), QStringLiteral("horse"))
        << OsmTag(QStringLiteral("route"), QStringLiteral("ski")) << OsmTag(QStringLiteral("roundtrip"), QStringLiteral("yes"))
        << OsmTag(QStringLiteral("network"), QStringLiteral("")) << OsmTag(QStringLiteral("ref"), QStringLiteral(""))
        << OsmTag(QStringLiteral("operator"), QStringLiteral(""))

        // Relations: route_master
        << OsmTag(QStringLiteral("route_master"), QStringLiteral("train")) << OsmTag(QStringLiteral("route_master"), QStringLiteral("subway"))
        << OsmTag(QStringLiteral("route_master"), QStringLiteral("monorail")) << OsmTag(QStringLiteral("route_master"), QStringLiteral("tram"))
        << OsmTag(QStringLiteral("route_master"), QStringLiteral("bus")) << OsmTag(QStringLiteral("route_master"), QStringLiteral("trolleybus"))
        << OsmTag(QStringLiteral("route_master"), QStringLiteral("ferry"))
        << OsmTag(QStringLiteral("route_master"), QStringLiteral("bicycle"))

        // Relations: public_transport
        << OsmTag(QStringLiteral("public_transport"), QStringLiteral("stop_area"))
        << OsmTag(QStringLiteral("public_transport"), QStringLiteral("stop_area_group"))

        // Relations: waterway
        << OsmTag(QStringLiteral("waterway"), QStringLiteral("river")) << OsmTag(QStringLiteral("waterway"), QStringLiteral("stream"))
        << OsmTag(QStringLiteral("waterway"), QStringLiteral("canal")) << OsmTag(QStringLiteral("waterway"), QStringLiteral("drain"))
        << OsmTag(QStringLiteral("waterway"), QStringLiteral("ditch"))

        // Relations: enforcement
        << OsmTag(QStringLiteral("enforcement"), QStringLiteral("maxheight")) << OsmTag(QStringLiteral("enforcement"), QStringLiteral("maxweight"))
        << OsmTag(QStringLiteral("enforcement"), QStringLiteral("maxspeed")) << OsmTag(QStringLiteral("enforcement"), QStringLiteral("mindistance"))
        << OsmTag(QStringLiteral("enforcement"), QStringLiteral("traffic_signals")) << OsmTag(QStringLiteral("enforcement"), QStringLiteral("check"))
        << OsmTag(QStringLiteral("enforcement"), QStringLiteral("access"))
        << OsmTag(QStringLiteral("enforcement"), QStringLiteral("toll"))

        // Others
        << OsmTag(QStringLiteral("height"), QStringLiteral("")) << OsmTag(QStringLiteral("rooms"), QStringLiteral(""))
        << OsmTag(QStringLiteral("beds"), QStringLiteral("")) << OsmTag(QStringLiteral("wheelchair"), QStringLiteral(""))
        << OsmTag(QStringLiteral("website"), QStringLiteral("")) << OsmTag(QStringLiteral("email"), QStringLiteral(""))
        << OsmTag(QStringLiteral("fee"), QStringLiteral("")) << OsmTag(QStringLiteral("destination"), QStringLiteral(""))
        << OsmTag(QStringLiteral("indoor"), QStringLiteral("yes"))

        // Recommended for all
        << OsmTag(QStringLiteral("addr:street"), QStringLiteral("")) << OsmTag(QStringLiteral("addr:housenumber"), QStringLiteral(""))
        << OsmTag(QStringLiteral("addr:postcode"), QStringLiteral("")) << OsmTag(QStringLiteral("addr:country"), QStringLiteral(""))
        << OsmTag(QStringLiteral("access"), QStringLiteral("private")) << OsmTag(QStringLiteral("access"), QStringLiteral("permissive"));

    return additionalOsmTags;
}

}
