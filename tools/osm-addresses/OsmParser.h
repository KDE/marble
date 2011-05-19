//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_OSMPARSER_H
#define MARBLE_OSMPARSER_H

#include "Writer.h"
#include "OsmRegion.h"
#include "OsmPlacemark.h"
#include "OsmRegionTree.h"

#include "marble/GeoDataLineString.h"
#include "marble/GeoDataPolygon.h"

#include <QtCore/QObject>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QPair>

namespace Marble
{

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
    QString name;
    Marble::GeoDataPolygon geometry;
    OsmRegion region;
    OsmOsmRegion* parent;
    int adminLevel;

    OsmOsmRegion() : parent( 0 ), adminLevel( 0 ) {}
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

struct Node : public Element {
    float lon;
    float lat;

    operator OsmPlacemark() const;
};

struct Way : public Element {
    QList<int> nodes;
    bool isBuilding;

    operator OsmPlacemark() const;
    void setPosition( const QMap<int, Node> &database, OsmPlacemark &placemark ) const;
    void setRegion( const QMap<int, Node> &database, const OsmRegionTree & tree, QList<OsmOsmRegion> & osmOsmRegions, OsmPlacemark &placemark ) const;
};

struct WayMerger {
public:
    QList<Way> ways;

    WayMerger( const Way &way ) {
        ways << way;
    }

    bool compatible( const Way &aWay ) const {
        foreach( const Way & way, ways ) {
            if ( way.nodes.first() == aWay.nodes.first() ) return true;
            if ( way.nodes.last()  == aWay.nodes.first() ) return true;
            if ( way.nodes.first() == aWay.nodes.last() ) return true;
            if ( way.nodes.last()  == aWay.nodes.last() ) return true;
        }

        return false;
    }

    bool compatible( const WayMerger &other ) const {
        foreach( const Way & way, ways ) {
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

class OsmParser : public QObject
{
    Q_OBJECT
public:
    explicit OsmParser( QObject *parent = 0 );

    void addWriter( Writer* writer );

    void read( const QFileInfo &file, const QString &areaName );

protected:
    virtual bool parse( const QFileInfo &file ) = 0;

    bool shouldSave( ElementType type, const QString &key, const QString &value );

    void setCategory( Element &element, const QString &key, const QString &value );

    QMap<int, Node> m_nodes;

    QMap<int, Way> m_ways;

    QMap<int, Relation> m_relations;

private:
    void importMultipolygon( const Relation &relation );

    void importWay( QVector<Marble::GeoDataLineString> &ways, int id );

    void writeOutlineKml( const QString &area ) const;

    QList< QList<Way> > merge( const QList<Way> &ways ) const;

    template<class T, class S>
    bool contains( const T &outer, const T &inner ) const {
        for ( int i = 0; i < inner.size(); ++i ) {
            if ( !outer.contains( inner[i] ) ) {
                return false;
            }
        }

        return true;
    }

    QColor randomColor() const;

    Marble::GeoDataLineString reverse( const Marble::GeoDataLineString & string );

    QList<Writer*> m_writers;

    QList<OsmOsmRegion> m_osmOsmRegions;

    QList<OsmPlacemark> m_placemarks;

    QMap<QString, OsmPlacemark::OsmCategory> m_categoryMap;
};

}

#endif // MARBLE_OSMPARSER_H
