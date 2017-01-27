//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSMPARSER_H
#define MARBLE_OSMPARSER_H

#include "Writer.h"
#include "OsmRegion.h"
#include "OsmPlacemark.h"
#include "OsmRegionTree.h"

#include <QObject>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QPair>

namespace Marble
{

class GeoDataLineString;

enum ElementType {
    NoType,
    NodeType,
    WayType,
    RelationType
};

enum RelationRole {
    None,
    Outer,
    Inner
};

struct OsmOsmRegion {
    OsmOsmRegion* parent;
    OsmRegion region;

    OsmOsmRegion() : parent( 0 ) {}
};

struct Element {
    bool save;
    QString name;
    QString street;
    QString houseNumber;
    QString city;
    OsmPlacemark::OsmCategory category;

    Element() : save( false ),
        category( OsmPlacemark::UnknownCategory ) {}
};

struct Coordinate {
    float lon;
    float lat;

    Coordinate(float lon=0.0, float lat=0.0);
};

struct Node : public Element {
    float lon;
    float lat;

    operator OsmPlacemark() const;
    operator Coordinate() const;
};

struct Way : public Element {
    QList<int> nodes;
    bool isBuilding;

    operator OsmPlacemark() const;
    void setPosition( const QHash<int, Coordinate> &database, OsmPlacemark &placemark ) const;
    void setRegion( const QHash<int, Node> &database, const OsmRegionTree & tree, QList<OsmOsmRegion> & osmOsmRegions, OsmPlacemark &placemark ) const;
};

struct WayMerger {
public:
    QList<Way> ways;

    WayMerger( const Way &way ) {
        ways << way;
    }

    bool compatible( const Way &aWay ) const {
        for( const Way & way: ways ) {
            if ( way.nodes.first() == aWay.nodes.first() ) return true;
            if ( way.nodes.last()  == aWay.nodes.first() ) return true;
            if ( way.nodes.first() == aWay.nodes.last() ) return true;
            if ( way.nodes.last()  == aWay.nodes.last() ) return true;
        }

        return false;
    }

    bool compatible( const WayMerger &other ) const {
        for( const Way & way: ways ) {
            if ( other.compatible( way ) ) {
                return true;
            }
        }

        return false;
    }

    void merge( const WayMerger &other ) {
        ways << other.ways;
    }
};

struct Relation : public Element {
    QList<int> nodes;
    QList< QPair<int, RelationRole> > ways;
    QList<int> relations;
    QString name;
    bool isMultipolygon;
    bool isAdministrativeBoundary;
    int adminLevel;

    Relation() : isMultipolygon( false ),
        isAdministrativeBoundary( false ),
        adminLevel( 0 )
    {
        // nothing to do
    }
};

struct Statistic {
    unsigned int mergedWays;
    unsigned int uselessWays;

    Statistic() : mergedWays( 0 ),
        uselessWays( 0 )
    {}
};

class OsmParser : public QObject
{
    Q_OBJECT
public:
    explicit OsmParser( QObject *parent = 0 );

    void addWriter( Writer* writer );

    void read( const QFileInfo &file, const QString &areaName );

    void writeKml( const QString &area, const QString &version, const QString &date, const QString &transport, const QString &payload, const QString &outputKml ) const;

protected:
    virtual bool parse( const QFileInfo &file, int pass, bool &needAnotherPass ) = 0;

    bool shouldSave( ElementType type, const QString &key, const QString &value );

    void setCategory( Element &element, const QString &key, const QString &value );

    QHash<int, Coordinate> m_coordinates;

    QHash<int, Node> m_nodes;

    QHash<int, Way> m_ways;

    QHash<int, Relation> m_relations;

private:
    GeoDataLinearRing *convexHull() const;

    void importMultipolygon( const Relation &relation );

    void importWay( QVector<Marble::GeoDataLineString> &ways, int id );

    QList< QList<Way> > merge( const QList<Way> &ways ) const;

    template<class T, class S>
    bool contains( const T &outer, const S &inner ) const {
        for ( int i = 0; i < inner.size(); ++i ) {
            if ( !outer.contains( inner[i] ) ) {
                bool onBorder = false;
                for ( int k=0; k<outer.size(); ++k ) {
                    if ( inner[i] == outer[k] ) {
                        onBorder = true;
                        break;
                    }
                }
                if ( !onBorder ) {
                    return false;
                }
            }
        }

        return true;
    }

    QColor randomColor() const;

    Marble::GeoDataLineString reverse( const Marble::GeoDataLineString & string );

    QList<Writer*> m_writers;

    QList<OsmOsmRegion> m_osmOsmRegions;

    QList<OsmPlacemark> m_placemarks;

    QHash<QString, OsmPlacemark::OsmCategory> m_categoryMap;

    mutable Statistic m_statistic;

    GeoDataLinearRing* m_convexHull;
};

}

#endif // MARBLE_OSMPARSER_H
