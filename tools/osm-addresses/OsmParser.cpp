//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmParser.h"
#include "OsmRegionTree.h"

#include "marble/GeoDataLinearRing.h"
#include "marble/GeoDataLineString.h"
#include "marble/GeoDataPolygon.h"
#include "marble/GeoDataDocument.h"
#include "marble/GeoDataFolder.h"
#include "marble/GeoDataPlacemark.h"
#include "marble/GeoDataMultiGeometry.h"
#include "marble/GeoDataStyle.h"
#include "marble/GeoDataStyleMap.h"
#include "marble/GeoDataLineStyle.h"
#include "marble/GeoDataFeature.h"
#include "geodata/writer/GeoWriter.h"
#include "geodata/data/GeoDataExtendedData.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>

namespace Marble
{

namespace {
    struct GrahamScanHelper {
        Coordinate coordinate;
        qreal direction;

        GrahamScanHelper( const Coordinate &coordinate_=Coordinate(), qreal direction_=0.0 )
            : coordinate( coordinate_ ), direction( direction_ )
        {
            // nothing to do
        }

        double turnDirection( const GrahamScanHelper &two, const GrahamScanHelper &three )
        {
            return    ( two.coordinate.lat - coordinate.lat ) * ( three.coordinate.lon - coordinate.lon )
                    - ( two.coordinate.lon - coordinate.lon ) * ( three.coordinate.lat - coordinate.lat );
        }

        static bool directionLessThan( const GrahamScanHelper &one, const GrahamScanHelper &two )
        {
            return one.direction < two.direction;
        }
    };
}

bool moreImportantAdminArea( const OsmRegion &a, const OsmRegion b )
{
    return a.adminLevel() < b.adminLevel();
}

OsmParser::OsmParser( QObject *parent ) :
    QObject( parent ), m_convexHull( 0 )
{
    m_categoryMap["tourism/camp_site"] = OsmPlacemark::AccomodationCamping;
    m_categoryMap["tourism/hostel"] = OsmPlacemark::AccomodationHostel;
    m_categoryMap["tourism/hotel"] = OsmPlacemark::AccomodationHotel;
    m_categoryMap["tourism/motel"] = OsmPlacemark::AccomodationMotel;
    //m_categoryMap["/"] = OsmPlacemark::AccomodationYouthHostel;
    m_categoryMap["amenity/library"] = OsmPlacemark::AmenityLibrary;
    m_categoryMap["amenity/college"] = OsmPlacemark::EducationCollege;
    m_categoryMap["amenity/school"] = OsmPlacemark::EducationSchool;
    m_categoryMap["amenity/university"] = OsmPlacemark::EducationUniversity;
    m_categoryMap["amenity/bar"] = OsmPlacemark::FoodBar;
    m_categoryMap["amenity/biergarten"] = OsmPlacemark::FoodBiergarten;
    m_categoryMap["amenity/cafe"] = OsmPlacemark::FoodCafe;
    m_categoryMap["amenity/fast_food"] = OsmPlacemark::FoodFastFood;
    m_categoryMap["amenity/pub"] = OsmPlacemark::FoodPub;
    m_categoryMap["amenity/restaurant"] = OsmPlacemark::FoodRestaurant;
    m_categoryMap["amenity/doctor"] = OsmPlacemark::HealthDoctors;
    m_categoryMap["amenity/doctors"] = OsmPlacemark::HealthDoctors;
    m_categoryMap["amenity/hospital"] = OsmPlacemark::HealthHospital;
    m_categoryMap["amenity/pharmacy"] = OsmPlacemark::HealthPharmacy;
    m_categoryMap["amenity/atm"] = OsmPlacemark::MoneyAtm;
    m_categoryMap["amenity/bank"] = OsmPlacemark::MoneyBank;
    m_categoryMap["shop/beverages"] = OsmPlacemark::ShoppingBeverages;
    m_categoryMap["shop/hifi"] = OsmPlacemark::ShoppingHifi;
    m_categoryMap["shop/supermarket"] = OsmPlacemark::ShoppingSupermarket;
    m_categoryMap["tourism/attraction"] = OsmPlacemark::TouristAttraction;
    m_categoryMap["tourism/castle"] = OsmPlacemark::TouristCastle;
    m_categoryMap["amenity/cinema"] = OsmPlacemark::TouristCinema;
    m_categoryMap["tourism/monument"] = OsmPlacemark::TouristMonument;
    m_categoryMap["tourism/museum"] = OsmPlacemark::TouristMuseum;
    m_categoryMap["historic/ruins"] = OsmPlacemark::TouristRuin;
    m_categoryMap["amenity/theatre"] = OsmPlacemark::TouristTheatre;
    m_categoryMap["tourism/theme_park"] = OsmPlacemark::TouristThemePark;
    m_categoryMap["tourism/viewpoint"] = OsmPlacemark::TouristViewPoint;
    m_categoryMap["tourism/zoo"] = OsmPlacemark::TouristZoo;
    m_categoryMap["aeroway/aerodrome"] = OsmPlacemark::TransportAirport;
    m_categoryMap["aeroway/terminal"] = OsmPlacemark::TransportAirportTerminal;
    m_categoryMap["amenity/bus_station"] = OsmPlacemark::TransportBusStation;
    m_categoryMap["highway/bus_stop"] = OsmPlacemark::TransportBusStop;
    m_categoryMap["highway/speed_camera"] = OsmPlacemark::TransportSpeedCamera;
    m_categoryMap["amenity/car_sharing"] = OsmPlacemark::TransportCarShare;
    m_categoryMap["amenity/car_rental"] = OsmPlacemark::TransportRentalCar;
    m_categoryMap["amenity/bicycle_rental"] = OsmPlacemark::TransportRentalBicycle;
    m_categoryMap["amenity/fuel"] = OsmPlacemark::TransportFuel;
    m_categoryMap["amenity/parking"] = OsmPlacemark::TransportParking;
    m_categoryMap["amenity/taxi"] = OsmPlacemark::TransportTaxiRank;
    m_categoryMap["railway/station"] = OsmPlacemark::TransportTrainStation;
    m_categoryMap["railway/tram_stop"] = OsmPlacemark::TransportTramStop;
}

void OsmParser::addWriter( Writer* writer )
{
    m_writers.push_back( writer );
}

Node::operator OsmPlacemark() const
{
    OsmPlacemark placemark;
    placemark.setCategory( category );
    placemark.setName( name.trimmed() );
    placemark.setHouseNumber( houseNumber.trimmed() );
    placemark.setLongitude( lon );
    placemark.setLatitude( lat );
    return placemark;
}

Node::operator Coordinate() const
{
    Coordinate coordinate;
    coordinate.lon = lon;
    coordinate.lat = lat;
    return coordinate;
}

Way::operator OsmPlacemark() const
{
    OsmPlacemark placemark;
    placemark.setCategory( category );
    placemark.setName( name.trimmed() );
    placemark.setHouseNumber( houseNumber.trimmed() );
    return placemark;
}

void Way::setPosition( const QHash<int, Coordinate> &database, OsmPlacemark &placemark ) const
{
    if ( !nodes.isEmpty() ) {
        if ( nodes.first() == nodes.last() && database.contains( nodes.first() ) ) {
            GeoDataLinearRing ring;
            foreach( int id, nodes ) {
                if ( database.contains( id ) ) {
                    const Coordinate &node = database[id];
                    GeoDataCoordinates coordinates( node.lon, node.lat, 0.0, GeoDataCoordinates::Degree );
                    ring << coordinates;
                } else {
                    qDebug() << "Missing node " << id << " in database";
                }
            }

            if ( !ring.isEmpty() ) {
                GeoDataCoordinates center = ring.latLonAltBox().center();
                placemark.setLongitude( center.longitude( GeoDataCoordinates::Degree ) );
                placemark.setLatitude( center.latitude( GeoDataCoordinates::Degree ) );
            }
        } else {
            int id = nodes.at( nodes.size() / 2 );
            if ( database.contains( id ) ) {
                const Coordinate &node = database[id];
                placemark.setLongitude( node.lon );
                placemark.setLatitude( node.lat );
            }
        }
    }
}

void Way::setRegion( const QHash<int, Node> &database,  const OsmRegionTree & tree, QList<OsmOsmRegion> & osmOsmRegions, OsmPlacemark &placemark ) const
{
    if ( !city.isEmpty() ) {
        foreach( const OsmOsmRegion & region, osmOsmRegions ) {
            if ( region.region.name() == city ) {
                placemark.setRegionId( region.region.identifier() );
                return;
            }
        }

        foreach( const Node & node, database ) {
            if ( node.category >= OsmPlacemark::PlacesRegion &&
                    node.category <= OsmPlacemark::PlacesIsland &&
                    node.name == city ) {
                qDebug() << "Creating a new implicit region from " << node.name << " at " << node.lon << "," << node.lat;
                OsmOsmRegion region;
                region.region.setName( city );
                region.region.setLongitude( node.lon );
                region.region.setLatitude( node.lat );
                placemark.setRegionId( region.region.identifier() );
                osmOsmRegions.push_back( region );
                return;
            }
        }

        qDebug() << "Unable to locate city " << city << ", setting it up without coordinates";
        OsmOsmRegion region;
        region.region.setName( city );
        placemark.setRegionId( region.region.identifier() );
        osmOsmRegions.push_back( region );
        return;
    }

    GeoDataCoordinates position( placemark.longitude(), placemark.latitude(), 0.0, GeoDataCoordinates::Degree );
    placemark.setRegionId( tree.smallestRegionId( position ) );
}

void OsmParser::read( const QFileInfo &content, const QString &areaName )
{
    QTime timer;
    timer.start();

    m_nodes.clear();
    m_ways.clear();
    m_relations.clear();

    m_placemarks.clear();
    m_osmOsmRegions.clear();

    int pass = 0;
    bool needAnotherPass = false;
    do {
        qWarning() << "Step 1." << pass << ": Parsing input file " << content.fileName();
        parse( content, pass++, needAnotherPass );
    }
    while ( needAnotherPass );

    qWarning() << "Step 2: " << m_coordinates.size() << "coordinates."
               << "Now extracting regions from" << m_relations.size() << "relations";

    foreach( const Relation & relation, m_relations.values() ) {
        if ( relation.isAdministrativeBoundary /*&& relation.isMultipolygon*/ ) {
            importMultipolygon( relation );
            if ( !relation.relations.isEmpty() ) {
                qDebug() << "Ignoring relations inside the relation " << relation.name;
            }
        }
    }

    m_relations.clear();

    for ( int i = 0; i < m_osmOsmRegions.size(); ++i ) {
        OsmOsmRegion &osmOsmRegion = m_osmOsmRegions[i];
        GeoDataCoordinates center = osmOsmRegion.region.geometry().latLonAltBox().center();
        osmOsmRegion.region.setLongitude( center.longitude( GeoDataCoordinates::Degree ) );
        osmOsmRegion.region.setLatitude( center.latitude( GeoDataCoordinates::Degree ) );
    }

    qWarning() << "Step 3: Creating region hierarchies from" << m_osmOsmRegions.size() << "administrative boundaries";

    QMultiMap<int,int> sortedRegions;
    for ( int i = 0; i < m_osmOsmRegions.size(); ++i ) {
        sortedRegions.insert( m_osmOsmRegions[i].region.adminLevel(), i );
    }

    for ( int i = 0; i < m_osmOsmRegions.size(); ++i ) {
        GeoDataLinearRing const & ring = m_osmOsmRegions[i].region.geometry().outerBoundary();
        OsmOsmRegion* parent = 0;
        qDebug() << "Examining admin region " << i << " of " << m_osmOsmRegions.count();
        for ( int level=m_osmOsmRegions[i].region.adminLevel()-1; level >= 0 && parent == 0; --level ) {
            QList<int> candidates = sortedRegions.values( level );
            qDebug() << "Examining " << candidates.count() << "admin regions on level" << level;
            foreach( int j, candidates ) {
                GeoDataLinearRing const & outer = m_osmOsmRegions[j].region.geometry().outerBoundary();
                if ( contains<GeoDataLinearRing, GeoDataLinearRing>( outer, ring ) ) {
                    if ( parent == 0 || contains<GeoDataLinearRing, GeoDataLinearRing>( parent->region.geometry().outerBoundary(), outer ) ) {
                        qDebug() << "Parent found: " << m_osmOsmRegions[i].region.name() << ", level " << m_osmOsmRegions[i].region.adminLevel()
                                   << "is a child of " << m_osmOsmRegions[j].region.name() << ", level " << m_osmOsmRegions[j].region.adminLevel();
                        parent = &m_osmOsmRegions[j];
                        break;
                    }
                }
            }
        }

        m_osmOsmRegions[i].parent = parent;
    }

    for ( int i = 0; i < m_osmOsmRegions.size(); ++i ) {
        int const parent = m_osmOsmRegions[i].parent ? m_osmOsmRegions[i].parent->region.identifier() : 0;
        m_osmOsmRegions[i].region.setParentIdentifier( parent );
    }

    OsmRegion mainArea;
    mainArea.setIdentifier( 0 );
    mainArea.setName( areaName );
    mainArea.setAdminLevel( 1 );
    QPair<float, float> minLon( -180.0, 180.0 ), minLat( -90.0, 90.0 );
    foreach( const Coordinate & node, m_coordinates ) {
        minLon.first  = qMin( node.lon, minLon.first );
        minLon.second = qMax( node.lon, minLon.second );
        minLat.first  = qMin( node.lat, minLat.first );
        minLat.second = qMax( node.lat, minLat.second );
    }
    GeoDataLatLonBox center( minLat.second, minLat.first,
                             minLon.second, minLon.first );
    mainArea.setLongitude( center.center().longitude( GeoDataCoordinates::Degree ) );
    mainArea.setLatitude( center.center().latitude( GeoDataCoordinates::Degree ) );

    QList<OsmRegion> regions;
    foreach( const OsmOsmRegion & region, m_osmOsmRegions ) {
        regions << region.region;
    }

    qSort( regions.begin(), regions.end(), moreImportantAdminArea );
    OsmRegionTree regionTree( mainArea );
    regionTree.append( regions );
    Q_ASSERT( regions.isEmpty() );
    int left = 0;
    regionTree.traverse( left );

    qWarning() << "Step 4: Creating placemarks from" << m_nodes.size() << "nodes";

    foreach( const Node & node, m_nodes ) {
        if ( node.save ) {
            OsmPlacemark placemark = node;
            GeoDataCoordinates position( node.lon, node.lat, 0.0, GeoDataCoordinates::Degree );
            placemark.setRegionId( regionTree.smallestRegionId( position ) );

            if ( !node.name.isEmpty() ) {
                placemark.setHouseNumber( QString() );
                m_placemarks.push_back( placemark );
            }

            if ( !node.street.isEmpty() && node.name != node.street ) {
                placemark.setCategory( OsmPlacemark::Address );
                placemark.setName( node.street.trimmed() );
                placemark.setHouseNumber( node.houseNumber.trimmed() );
                m_placemarks.push_back( placemark );
            }
        }
    }

    qWarning() << "Step 5: Creating placemarks from" << m_ways.size() << "ways";
    QMultiMap<QString, Way> waysByName;
    foreach ( const Way & way, m_ways ) {
        if ( way.save ) {
            if ( !way.name.isEmpty() && !way.nodes.isEmpty() ) {
                waysByName.insert( way.name, way );
            }

            if ( !way.street.isEmpty() && way.name != way.street && !way.nodes.isEmpty() ) {
                waysByName.insert( way.street, way );
            }
        } else {
            ++m_statistic.uselessWays;
        }
    }

    QSet<QString> keys = QSet<QString>::fromList( waysByName.keys() );
    foreach( const QString & key, keys ) {
        QList<QList<Way> > merged = merge( waysByName.values( key ) );
        foreach( const QList<Way> ways, merged ) {
            Q_ASSERT( !ways.isEmpty() );
            OsmPlacemark placemark = ways.first();
            ways.first().setPosition( m_coordinates, placemark );
            ways.first().setRegion( m_nodes, regionTree, m_osmOsmRegions, placemark );

            if ( placemark.category() != OsmPlacemark::Address && !ways.first().name.isEmpty() ) {
                placemark.setHouseNumber( QString() );
                m_placemarks.push_back( placemark );
            }

            if ( !ways.first().isBuilding || !ways.first().houseNumber.isEmpty() ) {
                placemark.setCategory( OsmPlacemark::Address );
                QString name = ways.first().street.isEmpty() ? ways.first().name : ways.first().street;
                if ( !name.isEmpty() ) {
                    placemark.setName( name.trimmed() );
                    placemark.setHouseNumber( ways.first().houseNumber.trimmed() );
                    m_placemarks.push_back( placemark );
                }
            }
        }
    }

    m_convexHull = convexHull();
    m_coordinates.clear();
    m_nodes.clear();
    m_ways.clear();

    Q_ASSERT( regions.isEmpty() );
    foreach( const OsmOsmRegion & region, m_osmOsmRegions ) {
        regions << region.region;
    }

    qSort( regions.begin(), regions.end(), moreImportantAdminArea );
    regionTree = OsmRegionTree( mainArea );
    regionTree.append( regions );
    Q_ASSERT( regions.isEmpty() );
    left = 0;
    regionTree.traverse( left );
    regions = regionTree;

    qWarning() << "Step 6: " << m_statistic.mergedWays << " ways merged," << m_statistic.uselessWays << "useless ways."
               << "Now serializing" << regions.size() << "regions";
    foreach( const OsmRegion & region, regions ) {
        foreach( Writer * writer, m_writers ) {
            writer->addOsmRegion( region );
        }
    }

    qWarning() << "Step 7: Serializing" << m_placemarks.size() << "placemarks";
    foreach( const OsmPlacemark & placemark, m_placemarks ) {
        foreach( Writer * writer, m_writers ) {
            Q_ASSERT( !placemark.name().isEmpty() );
            writer->addOsmPlacemark( placemark );
        }
    }

    qWarning() << "Step 8: There is no step 8. Done after " << timer.elapsed() / 1000 << "s.";
    //writeOutlineKml( areaName );
}

QList< QList<Way> > OsmParser::merge( const QList<Way> &ways ) const
{
    QList<WayMerger> mergers;

    foreach( const Way & way, ways ) {
        mergers << WayMerger( way );
    }

    bool moved = false;
    do {
        moved = false;
        for( int i = 0; i < mergers.size(); ++i ) {
            for ( int j = i + 1; j < mergers.size(); ++j ) {
                if ( mergers[i].compatible( mergers[j] ) ) {
                    mergers[i].merge( mergers[j] );
                    moved = true;
                    mergers.removeAt( j );
                }
            }
        }
    } while ( moved );

    QList< QList<Way> > result;
    foreach( const WayMerger & merger, mergers ) {
        result << merger.ways;
    }
    m_statistic.mergedWays += ( ways.size() - result.size() );

    return result;
}

void OsmParser::importMultipolygon( const Relation &relation )
{
    /** @todo: import nodes? What are they used for? */
    typedef QPair<int, RelationRole> RelationPair;
    QVector<GeoDataLineString> outer;
    QVector<GeoDataLineString> inner;
    foreach( const RelationPair & pair, relation.ways ) {
        if ( pair.second == Outer ) {
            importWay( outer, pair.first );
        } else if ( pair.second == Inner ) {
            importWay( inner, pair.first );
        } else {
            qDebug() << "Ignoring way " << pair.first << " with unknown relation role.";
        }
    }

    foreach( const GeoDataLineString & string, outer ) {
        if ( string.isEmpty() || !( string.first() == string.last() ) ) {
            qDebug() << "Ignoring open polygon in relation " << relation.name << ". Check data.";
            continue;
        }

        GeoDataPolygon polygon;
        polygon.setOuterBoundary( string );
        Q_ASSERT( polygon.outerBoundary().size() > 0 );

        foreach( const GeoDataLineString & hole, inner ) {
            if ( contains<GeoDataLinearRing, GeoDataLineString>( polygon.outerBoundary(), hole ) ) {
                polygon.appendInnerBoundary( hole );
            }
        }

        OsmOsmRegion region;
        region.region.setName( relation.name );
        region.region.setGeometry( polygon );
        region.region.setAdminLevel( relation.adminLevel );
        qDebug() << "Adding administrative region " << relation.name;
        m_osmOsmRegions.push_back( region );
    }
}

void OsmParser::importWay( QVector<GeoDataLineString> &ways, int id )
{
    if ( !m_ways.contains( id ) ) {
        qDebug() << "Skipping unknown way " << id << ". Check data.";
        return;
    }

    GeoDataLineString way;
    foreach( int node, m_ways[id].nodes ) {
        if ( !m_coordinates.contains( node ) ) {
            qDebug() << "Skipping unknown node " << node << ". Check data.";
        } else {
            const Coordinate &nd = m_coordinates[node];
            GeoDataCoordinates coordinates( nd.lon, nd.lat, 0.0, GeoDataCoordinates::Degree );
            way << coordinates;
        }
    }

    QList<int> remove;
    do {
        remove.clear();
        for ( int i = 0; i < ways.size(); ++i ) {
            const GeoDataLineString &existing = ways[i];
            if ( existing.first() == way.first() ) {
                way = reverse( way ) << existing;
                remove.push_front( i );
            } else if ( existing.last() == way.first() ) {
                GeoDataLineString copy = existing;
                way = copy << way;
                remove.push_front( i );
            } else if ( existing.first() == way.last() ) {
                way << existing;
                remove.push_front( i );
            } else if ( existing.last() == way.last() ) {
                way << reverse( existing );
                remove.push_front( i );
            }
        }

        foreach( int key, remove ) {
            ways.remove( key );
        }
    } while ( !remove.isEmpty() );

    ways.push_back( way );
}

GeoDataLineString OsmParser::reverse( const GeoDataLineString & string )
{
    GeoDataLineString result;
    for ( int i = string.size() - 1; i >= 0; --i ) {
        result << string[i];
    }
    return result;
}

bool OsmParser::shouldSave( ElementType /*type*/, const QString &key, const QString &value )
{
    typedef QList<QString> Dictionary;
    static QHash<QString, Dictionary> interestingTags;
    if ( interestingTags.isEmpty() ) {
        Dictionary highways;
        highways << "primary" << "secondary" << "tertiary";
        highways << "residential" << "unclassified" << "road";
        highways << "living_street" << "service" << "track";
        highways << "bus_stop" << "platform" << "speed_camera";
        interestingTags["highway"] = highways;

        Dictionary aeroways;
        aeroways << "aerodrome" << "terminal";
        interestingTags["aeroway"] = aeroways;

        interestingTags["aerialway"] = Dictionary() << "station";

        Dictionary leisures;
        leisures << "sports_centre" << "stadium" << "pitch";
        leisures << "park" << "dance";
        interestingTags["leisure"] = leisures;

        Dictionary amenities;
        amenities << "restaurant" << "food_court" << "fast_food";
        amenities << "pub" << "bar" << "cafe";
        amenities << "biergarten" << "kindergarten" << "school";
        amenities << "college" << "university" << "library";
        amenities << "ferry_terminal" << "bus_station" << "car_rental";
        amenities << "car_sharing" << "fuel" << "parking";
        amenities << "bank" << "pharmacy" << "hospital";
        amenities << "cinema" << "nightclub" << "theatre";
        amenities << "taxi" << "bicycle_rental" << "atm";
        interestingTags["amenity"] = amenities;

        Dictionary shops;
        shops << "beverages" << "supermarket" << "hifi";
        interestingTags["shop"] = shops;

        Dictionary tourism;
        tourism << "attraction" << "camp_site" << "caravan_site";
        tourism << "chalet" << "chalet" << "hostel";
        tourism << "hotel" << "motel" << "museum";
        tourism << "theme_park" << "viewpoint" << "zoo";
        interestingTags["tourism"] = tourism;

        Dictionary historic;
        historic << "castle" << "fort" << "monument";
        historic << "ruins";
        interestingTags["historic"] = historic;

        Dictionary railway;
        railway << "station" << "tram_stop";
        interestingTags["railway"] = railway;

        Dictionary places;
        places << "region" << "county" << "city";
        places << "town" << "village" << "hamlet";
        places << "isolated_dwelling" << "suburb" << "locality";
        places << "island";
        interestingTags["place"] = places;
    }

    return interestingTags.contains( key ) &&
           interestingTags[key].contains( value );
}

void OsmParser::setCategory( Element &element, const QString &key, const QString &value )
{
    QString const term = key + '/' + value;
    if ( m_categoryMap.contains( term ) ) {
        if ( element.category != OsmPlacemark::UnknownCategory ) {
            qDebug() << "Overwriting category " << element.category << " with " << m_categoryMap[term] << " for " << element.name;
        }
        element.category = m_categoryMap[term];
    }
}

// From http://en.wikipedia.org/wiki/Graham_scan#Pseudocode
GeoDataLinearRing* OsmParser::convexHull() const
{
    Q_ASSERT(m_coordinates.size()>2);
    QList<Coordinate> coordinates = m_coordinates.values();

    QVector<GrahamScanHelper> points;
    points.reserve( coordinates.size()+1 );
    Coordinate start = coordinates.first();
    int startPos = 0;
    for ( int i=0; i<coordinates.size(); ++i ) {
        if ( coordinates[i].lon < start.lon ) {
            start = coordinates[i];
            startPos = i;
        }
        points << coordinates[i];
    }

    int const n = points.size();
    Q_ASSERT( n == coordinates.size() );
    Q_ASSERT( n>2 );
    qSwap( points[1], points[startPos] );

    Q_ASSERT( start.lat != 360.0 );

    for ( int i=0; i<points.size(); ++i ) {
        points[i].direction = atan2( start.lon - points[i].coordinate.lon,
                                   start.lat - points[i].coordinate.lat );
    }

    qSort( points.begin(), points.end(), GrahamScanHelper::directionLessThan );
    points << points.first();

    int m = 2;
    for ( int i=3; i<=n; ++i ) {
        while ( points[m-1].turnDirection( points[m], points[i] ) <= 0 ) {
            if ( m == 2 ) {
                qSwap( points[m], points[i] );
                ++i;
            } else {
                --m;
            }

            Q_ASSERT( n+1 == points.size() );
            Q_ASSERT( m > 0 );
            Q_ASSERT( m <= n );
            Q_ASSERT( i >= 0 );
            Q_ASSERT( i <= n );
        }

        ++m;
        qSwap( points[m], points[i] );
    }

    GeoDataLinearRing* ring = new GeoDataLinearRing;
    for ( int i=1; i<=m; ++i ) {
        ring->append(GeoDataCoordinates(points[i].coordinate.lon, points[i].coordinate.lat, 0.0, GeoDataCoordinates::Degree));
    }

    return ring;
}

QColor OsmParser::randomColor() const
{
    QVector<QColor> colors = QVector<QColor>() << oxygenAluminumGray4 << oxygenBrickRed4;
    colors << oxygenBrownOrange4 << oxygenForestGreen4 << oxygenHotOrange4;
    colors << oxygenSeaBlue2 << oxygenSkyBlue4 << oxygenSunYellow6;
    return colors.at( qrand() % colors.size() );
}

void OsmParser::writeKml( const QString &area, const QString &version, const QString &date, const QString &transport, const QString &payload, const QString &filename ) const
{
    GeoDataDocument* document = new GeoDataDocument;

    //foreach( const OsmOsmRegion & region, m_osmOsmRegions ) {
    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setName( area );
    if ( !version.isEmpty() ) {
        placemark->extendedData().addValue( GeoDataData( "version", version ) );
    }
    if ( !date.isEmpty() ) {
        placemark->extendedData().addValue( GeoDataData( "date", date ) );
    }
    if ( !transport.isEmpty() ) {
        placemark->extendedData().addValue( GeoDataData( "transport", transport ) );
    }
    if ( !payload.isEmpty() ) {
        placemark->extendedData().addValue( GeoDataData( "payload", payload ) );
    }

    GeoDataStyle style;
    GeoDataLineStyle lineStyle;
    QColor color = randomColor();
    color.setAlpha( 200 );
    lineStyle.setColor( color );
    lineStyle.setWidth( 4 );
    style.setLineStyle( lineStyle );
    style.setStyleId( color.name().replace( '#', 'f' ) );

    GeoDataStyleMap styleMap;
    styleMap.setStyleId( color.name().replace( '#', 'f' ) );
    styleMap.insert( "normal", QString( "#" ).append( style.styleId() ) );
    document->addStyle( style );

    placemark->setStyleUrl( QString( "#" ).append( styleMap.styleId() ) );

    //placemark->setGeometry( new GeoDataLinearRing( region.region.geometry().outerBoundary() ) );
    GeoDataMultiGeometry *geometry = new GeoDataMultiGeometry;
    geometry->append( m_convexHull );
    placemark->setGeometry( geometry );

    document->append( placemark );
    document->addStyleMap( styleMap );
//    }

    GeoWriter writer;
    writer.setDocumentType( "http://earth.google.com/kml/2.2" );

    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) ) {
        qCritical() << "Cannot write to " << file.fileName();
    }

    if ( !writer.write( &file, document ) ) {
        qCritical() << "Can not write to " << file.fileName();
    }
    file.close();
}

Coordinate::Coordinate(float lon_, float lat_) : lon(lon_), lat(lat_)
{
    // nothing to do
}

}
