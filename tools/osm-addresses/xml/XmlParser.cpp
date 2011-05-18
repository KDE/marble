//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "XmlParser.h"

#include <QtCore/QDebug>

namespace Marble
{

XmlParser::XmlParser( QObject *parent ) :
    OsmParser( parent )
{
    // nothing to do
}

bool XmlParser::parse( const QFileInfo &content )
{
    QXmlSimpleReader xmlReader;
    xmlReader.setContentHandler( this );
    xmlReader.setErrorHandler( this );

    QFile file( content.absoluteFilePath() );
    QXmlInputSource *source = new QXmlInputSource( &file );

    if ( !xmlReader.parse( source ) ) {
        qCritical() << "Failed to parse " << content.absoluteFilePath();
        return false;
    }

    return true;
}

bool XmlParser::startElement ( const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & atts )
{
    if ( qName == "node" ) {
        m_node = Node();
        m_id = atts.value( "id" ).toInt();
        m_node.lon = atts.value( "lon" ).toFloat();
        m_node.lat = atts.value( "lat" ).toFloat();
        m_element = NodeType;
    } else if ( qName == "way" ) {
        m_id = atts.value( "id" ).toInt();
        m_way = Way();
        m_element = WayType;
    } else if ( qName == "nd" ) {
        m_way.nodes.push_back( atts.value( "ref" ).toInt() );
    } else if ( qName == "relation" ) {
        m_id = atts.value( "id" ).toInt();
        m_relation = Relation();
        m_relation.nodes.clear();
        m_element = RelationType;
    } else if ( qName == "member" ) {
        if ( atts.value( "type" ) == "node" ) {
            m_relation.nodes.push_back( atts.value( "ref" ).toInt() );
        } else if ( atts.value( "type" ) == "way" ) {
            RelationRole role = None;
            if ( atts.value( "role" ) == "outer" ) role = Outer;
            if ( atts.value( "role" ) == "inner" ) role = Inner;
            m_relation.ways.push_back( QPair<int, RelationRole>( atts.value( "ref" ).toInt(), role ) );
        } else if ( atts.value( "type" ) == "relation" ) {
            m_relation.relations.push_back( atts.value( "ref" ).toInt() );
        } else {
            qDebug() << "Unknown relation member type " << atts.value( "type" );
        }
    } else if ( qName == "tag" && m_element == RelationType ) {
        if ( atts.value( "k" ) == "boundary" && atts.value( "v" ) == "administrative" ) {
            m_relation.isAdministrativeBoundary = true;
        } else if ( atts.value( "k" ) == "name" ) {
            m_relation.name = atts.value( "v" );
        } else if ( atts.value( "k" ) == "type" && atts.value( "v" ) == "multipolygon" ) {
            m_relation.isMultipolygon = true;
        }
    } else if ( qName == "tag" && m_element == WayType ) {
        QString const key = atts.value( "k" );
        QString const value = atts.value( "v" );
        if ( key == "name" ) {
            m_way.name = value;
        } else if ( key == "addr:street" ) {
            m_way.street = value;
            m_way.save = true;
        } else if ( key == "addr:housenumber" ) {
            m_way.houseNumber = value;
            m_way.save = true;
        } else if ( key == "addr:city" ) {
            m_way.city = value;
            m_way.save = true;
        } else if ( key == "building" && value == "yes" ) {
            m_way.isBuilding = true;
        } else  {
            if ( shouldSave( WayType, key, value ) ) {
                m_way.save = true;
            }
            setCategory( m_way, key, value );
        }
    } else if ( qName == "tag" && m_element == NodeType ) {
        QString const key = atts.value( "k" );
        QString const value = atts.value( "v" );
        if ( key == "name" ) {
            m_node.name = value;
        } else if ( key == "addr:street" ) {
            m_node.street = value;
            m_node.save = true;
        } else if ( key == "addr:housenumber" ) {
            m_node.houseNumber = value;
            m_node.save = true;
        } else if ( key == "addr:city" ) {
            m_node.city = value;
            m_node.save = true;
        } else {
            if ( shouldSave( NodeType, key, value ) ) {
                m_node.save = true;
            }
            setCategory( m_node, key, value );
        }
    }

    return true;
}

bool XmlParser::endElement ( const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName )
{
    if ( qName == "node" ) {
        m_nodes[m_id] = m_node;
    } else if ( qName == "way" ) {
        m_ways[m_id] = m_way;
    } else if ( qName == "relation" ) {
        m_relations[m_id] = m_relation;
    }

    return true;
}

}
