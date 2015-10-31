//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmTagEditorWidget_p.h"
#include "OsmTagEditorWidget.h"

// Marble
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "OsmPlacemarkData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"

// Qt
#include <QTreeWidget>
#include <QObject>

namespace Marble
{

const QString OsmTagEditorWidgetPrivate::m_customTagAdderText = QObject::tr( "Add custom tag..." );
OsmTagEditorWidgetPrivate::OsmTagEditorWidgetPrivate( OsmTagEditorWidget* q_)
    :q( q_ )
{
    // nothing to do
}

OsmTagEditorWidgetPrivate::~OsmTagEditorWidgetPrivate()
{
    // nothing to do
}

void OsmTagEditorWidgetPrivate::init( QWidget* parent )
{
    setupUi( parent );
    populatePresetTagsList();
    populateCurrentTagsList();
    m_recommendedTagsList->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_recommendedTagsList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_recommendedTagsList->setRootIsDecorated( false );

    m_currentTagsList->setSelectionBehavior( QAbstractItemView::SelectRows );
    m_currentTagsList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_currentTagsList->setRootIsDecorated( false );

    QObject::connect( m_addTagButton, SIGNAL( pressed() ),
                      q, SLOT( addSelectedTag() ) );
    QObject::connect( m_recommendedTagsList, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ),
                      q, SLOT( addSelectedTag() ) );
    QObject::connect( m_removeTagButton, SIGNAL( pressed() ),
                      q, SLOT( removeSelectedTag() ) );
    QObject::connect( m_currentTagsList, SIGNAL( itemChanged( QTreeWidgetItem*, int ) ),
                      q, SLOT( handleItemChanged( QTreeWidgetItem*, int ) ) );
    QObject::connect( m_currentTagsList, SIGNAL( itemDoubleClicked(QTreeWidgetItem*,int) ),
                      q, SLOT( handleDoubleClick( QTreeWidgetItem*, int) ) );
}

void OsmTagEditorWidgetPrivate::populateCurrentTagsList()
{
    // Name tag
    if ( !m_placemark->name().isEmpty() ) {
        QStringList itemText;

        // "name" is a standard OSM tag, don't translate
        itemText<< "name" << m_placemark->name();
        QTreeWidgetItem *nameTag = new QTreeWidgetItem( itemText );
        nameTag->setDisabled( true );
        m_currentTagsList->addTopLevelItem( nameTag );
    }

    // Multipolygon type tag
    if ( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        QStringList itemText;
        // "type" is a standard OSM tag, don't translate
        itemText<< "type" << "multipolygon";
        QTreeWidgetItem *typeTag = new QTreeWidgetItem( itemText );
        typeTag->setDisabled( true );
        m_currentTagsList->addTopLevelItem( typeTag );
    }

    // Other tags
    if( m_placemark->hasOsmData() ) {
        OsmPlacemarkData osmData = m_placemark->osmData();
        QHash< QString, QString>::const_iterator it = osmData.tagsBegin();
        QHash< QString, QString>::const_iterator end = osmData.tagsEnd();
        for ( ; it != end; ++it ) {
            QTreeWidgetItem *tagItem = tagWidgetItem( OsmPresetLibrary::OsmTag( it.key(), it.value() ) );
            m_currentTagsList->addTopLevelItem( tagItem );
        }
    }

    // Custom tag adder item
    QTreeWidgetItem *adderItem = new QTreeWidgetItem();
    adderItem->setText( 0, m_customTagAdderText );
    adderItem->setTextColor( 0, Qt::gray );
    adderItem->setIcon( 0, QIcon( ":marble/list-add.png" ) );
    adderItem->setFlags( adderItem->flags() | Qt::ItemIsEditable );
    m_currentTagsList->addTopLevelItem( adderItem );
    m_currentTagsList->resizeColumnToContents( 0 );
    m_currentTagsList->resizeColumnToContents( 1 );


}

void OsmTagEditorWidgetPrivate::populatePresetTagsList()
{
    QList<OsmTag> tags = recommendedTags();
    foreach ( const OsmPresetLibrary::OsmTag &tag, tags ) {
        QTreeWidgetItem *tagItem = tagWidgetItem( tag );
        m_recommendedTagsList->addTopLevelItem( tagItem );
    }
}

QTreeWidgetItem *OsmTagEditorWidgetPrivate::tagWidgetItem( const OsmTag &tag ) const
{
    QStringList itemText;

    itemText << tag.first;
    itemText << ( tag.second.isEmpty() ? QString( "<%1>" ).arg( QObject::tr( "value" ) ) : tag.second );

    QTreeWidgetItem *tagItem = new QTreeWidgetItem( itemText );

    // Getting the icon preset for the tag ( if there's one available )
    GeoDataStyle::ConstPtr style = OsmPresetLibrary::presetStyle( tag );
    QPixmap iconPixmap = QPixmap::fromImage( style->iconStyle().icon() );
    tagItem->setIcon( 1, QIcon( iconPixmap ) );

    return tagItem;
}

QList<OsmTagEditorWidgetPrivate::OsmTag> OsmTagEditorWidgetPrivate::recommendedTags() const
{
    QList<OsmTag> recommendedTags;

    QStringList filter = generateTagFilter();

    QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator it = OsmPresetLibrary::begin();
    QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator end = OsmPresetLibrary::end();

    for ( ; it != end; ++it ) {
        if ( filter.contains( it.key().first ) ) {
            recommendedTags += it.key();
        }
    }

    QList<OsmTag>::const_iterator additionalIt = OsmPresetLibrary::additionalTagsBegin();
    QList<OsmTag>::const_iterator additionalEnd = OsmPresetLibrary::additionalTagsEnd();

    for ( ; additionalIt != additionalEnd; ++additionalIt ) {
        if ( filter.contains( additionalIt->first ) ) {
            recommendedTags += *additionalIt;
        }
    }

    return recommendedTags;
}


QStringList OsmTagEditorWidgetPrivate::generateTagFilter() const
{
    // TO DO: implement more dynamic criteria for the filter
    // based on https://taginfo.openstreetmap.org/ and http://wiki.openstreetmap.org/wiki/

    // Contains all keys that should pass through the filter ( eg. { "amenity", "landuse", etc.. } )
    QStringList filter;

    bool condition;
    QStringList tags, tagsAux;
    QString type;
    if ( m_placemark->extendedData().value( "osmRelation" ).value().toString() == "yes" ) {
        type = "Relation";
    }
    else {
        type = m_placemark->geometry()->nodeType();
    }
    OsmPlacemarkData osmData;
    if ( m_placemark->hasOsmData() ) {
        osmData = m_placemark->osmData();
    }
    else {
        osmData = OsmPlacemarkData();
    }

    // Patterns in order of usefulness


    // If the placemark is a node, and it doesn't already have any node-specific tags, recommend all node-specific tags
    tags      = QStringList() << "amenity=*" << "shop=*" << "transport=*" << "tourism=*" << "historic=*" << "power=*" << "barrier=*";
    condition = ( type == GeoDataTypes::GeoDataPointType ) && !containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tags );
    }



    // If the placemark is a way, and it doesn't already have any way-specific tags, recommend all way-specific tags
    tags      = QStringList() << "highway=*" << "waterway=*" << "railway=*";
    condition = ( type == GeoDataTypes::GeoDataLineStringType ) && !containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tags );
    }



    // If the placemark is a polygon, and it doesn't already have any polygon-specific tags, recommend all polygon-specific tags
    tags      = QStringList() << "landuse=*" << "leisure=*";
    condition = ( type == GeoDataTypes::GeoDataPolygonType ) && !containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tags );
    }



    // If the placemark is a relation, recommend type=*
    tags      = QStringList() << "type=*";
    condition = type == "Relation";
    if ( condition ) {
        addPattern( filter, osmData, tags );
    }



    // If the placemark has type=route, recommend route=*, network=*, ref=*, operator=*
    tags      = QStringList() << "type=route";
    tagsAux   = QStringList() << "route=*" << "network=*" << "ref=*" << "operator=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has type=route_master, recommend route_master=*,
    tags      = QStringList() << "type=route_master";
    tagsAux   = QStringList() << "route_master=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has type=public_transport, recommend public_transport=*,
    tags      = QStringList() << "type=public_transport";
    tagsAux   = QStringList() << "public_transport=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has type=waterway, recommend waterway=*,
    tags      = QStringList() << "type=waterway";
    tagsAux   = QStringList() << "waterway=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has type=enforcement, recommend enforcement=*,
    tags      = QStringList() << "type=enforcement";
    tagsAux   = QStringList() << "enforcement=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has amenity=place_of_worship, recommend religion=*
    tags      = QStringList() << "amenity=place_of_worship";
    tagsAux   = QStringList() << "religion=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has amenity=toilets, recommend drinking_water=*, indoor=*
    tags      = QStringList() << "amenity=toilets";
    tagsAux   = QStringList() << "drinking_water=*" << "indoor=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has  tourism=hostel, tourism=hotel or tourism=motel, recommend rooms=*, beds=*, wheelchair=*
    tags      = QStringList() << "tourism=hotel" << "tourism=hostel" << "tourism=motel";
    tagsAux   = QStringList() << "rooms=*" << "beds=*" << "wheelchair=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has  tourism=*, shop=*, amenity=*, recommend website=*, email=*, fee=*
    tags      = QStringList() << "tourism=*" << "shop=*" << "amenity=*";
    tagsAux   = QStringList() << "website=*" << "email=*" << "fee=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has amenity=* shop=*, recommend building=*
    tags      = QStringList() << "amenity=*" << "shop=*";
    tagsAux   = QStringList() << "building=*";
    condition = containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark has highway=*, recommend "lanes=*", "maxspeed=*", "oneway=*", "service=*", "bridge=*", "tunnel=*"
    tags      = QStringList() << "highway=*";
    tagsAux   = QStringList() << "lanes=*" << "maxspeed=*" << "maxheight=*" << "maxweight=*" << "abutters=*" << "oneway=*" << "service=*" << "bridge=*" << "tunnel=*";
    condition = ( type == GeoDataTypes::GeoDataLineStringType ) && containsAny( osmData, tags );
    if ( condition ) {
        addPattern( filter, osmData, tagsAux );
    }



    // If the placemark is a polygon, recommend "surface=*"
    tags      = QStringList() << "surface=*";
    condition = ( type == GeoDataTypes::GeoDataPolygonType );
    if ( condition ) {
        addPattern( filter, osmData, tags );
    }



    // Always recommend these:
    tags      = QStringList() << "addr:street=*" << "addr:housenumber=*" << "addr:postcode=*" << "addr:country=*" << "access=*";
    addPattern( filter, osmData, tags );


    return filter;
}

bool OsmTagEditorWidgetPrivate::containsAny( const OsmPlacemarkData &osmData, const QStringList &tags ) const
{
    foreach ( const QString &tag, tags ) {
        QStringList tagSplit = tag.split( '=' );

        // Only "key=value" mappings should be checked
        Q_ASSERT( tagSplit.size() == 2  );

        QString key = tagSplit.at( 0 );
        QString value = tagSplit.at( 1 );

        if ( value == "*" && osmData.containsTagKey( key ) ) {
            return true;
        }
        else if ( value != "*" && osmData.containsTag( key, value ) ) {
            return true;
        }
    }
    return false;
}

void OsmTagEditorWidgetPrivate::addPattern( QStringList &filter, const OsmPlacemarkData &osmData, const QStringList &tags ) const
{
    foreach ( const QString &tag, tags ) {
        QStringList tagSplit = tag.split( '=' );
        QString key = tagSplit.at( 0 );
        if ( !osmData.containsTagKey( key ) ) {
            filter << key;
        }
    }
}

}
