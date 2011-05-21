//
// This file is part of the Marble Virtual Globe.
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//
// This file originates from the MoNav project where it was named pbfreader.h and
// Copyright 2010  Christian Vetter veaac.fdirct@gmail.com
//

#include "PbfParser.h"

#include <QtCore/QDebug>

#include <zlib.h>

#include <fstream>

#include <netinet/in.h>

using namespace std;
using namespace OSMPBF;

PbfParser::PbfParser() :
    m_currentGroup( 0 ),
    m_currentEntity( 0 ),
    m_loadBlock( false )
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

bool PbfParser::parse( const QFileInfo &fileInfo )
{
    QFile file( fileInfo.absoluteFilePath() );
    if ( !file.open( QFile::ReadOnly ) ) {
        qCritical() << "Unable to open file " << fileInfo.absoluteFilePath() << " for reading.";
        return false;
    }

    m_stream.setDevice( &file );
    m_stream.setByteOrder( QDataStream::BigEndian );

    if ( !parseBlobHeader() ) {
        return false;
    }

    if ( m_blobHeader.type() != "OSMHeader" ) {
        qCritical() << "Unable to parse blob header type " << m_blobHeader.type().c_str();
        return false;
    }

    if ( !parseBlob() ) {
        return false;
    }

    if ( !parseData() ) {
        return false;
    }

    m_loadBlock = true;

    while ( true ) {

        if ( m_loadBlock ) {
            if ( !readNext() ) {
                return true;
            }
            loadBlock();
            loadGroup();
        }

        switch ( m_mode ) {
        case ModeNode:
            parseNode();
            break;
        case ModeWay:
            parseWay();
            break;
        case ModeRelation:
            parseRelation();
            break;
        case ModeDense:
            parseDense();
            break;
        }
    }

    return true;
}

bool PbfParser::parseBlobHeader()
{
    int size( -1 );
    m_stream >> size;

    if ( size < 0 ) {
        qCritical() << "Invalid blob header size " << size;
        return false;
    }

    m_buffer.resize( size );
    int readBytes = m_stream.readRawData( m_buffer.data(), size );
    if ( readBytes != size ) {
        qCritical() << "Unable to read blob header";
        return false;
    }

    if ( !m_blobHeader.ParseFromArray( m_buffer.constData(), size ) ) {
        qCritical() << "Unable to parse blob header";
        return false;
    }

    return true;
}

bool PbfParser::parseBlob()
{
    int size = m_blobHeader.datasize();
    if ( size < 0 ) {
        qCritical() << "invalid blob size:" << size;
        return false;
    }

    m_buffer.resize( size );
    int readBytes = m_stream.readRawData( m_buffer.data(), size );
    if ( readBytes != size ) {
        qCritical() << "failed to read blob";
        return false;
    }

    if ( !m_blob.ParseFromArray( m_buffer.constData(), size ) ) {
        qCritical() << "failed to parse blob";
        return false;
    }

    if ( m_blob.has_raw() ) {
        const std::string& data = m_blob.raw();
        m_buffer.resize( data.size() );
        for ( unsigned int i = 0; i < data.size(); ++i ) {
            m_buffer[i] = data[i];
        }
    } else if ( m_blob.has_zlib_data() ) {
        m_buffer.resize( m_blob.raw_size() );
        z_stream zStream;
        zStream.next_in = ( unsigned char* ) m_blob.zlib_data().data();
        zStream.avail_in = m_blob.zlib_data().size();
        zStream.next_out = ( unsigned char* ) m_buffer.data();
        zStream.avail_out = m_blob.raw_size();
        zStream.zalloc = Z_NULL;
        zStream.zfree = Z_NULL;
        zStream.opaque = Z_NULL;
        int result = inflateInit( &zStream );
        if ( result != Z_OK ) {
            qCritical() << "failed to open zlib m_stream";
            return false;
        }
        result = inflate( &zStream, Z_FINISH );
        if ( result != Z_STREAM_END ) {
            qCritical() << "failed to inflate zlib m_stream";
            return false;
        }
        result = inflateEnd( &zStream );
        if ( result != Z_OK ) {
            qCritical() << "failed to close zlib m_stream";
            return false;
        }

        return true;
    } else if ( m_blob.has_lzma_data() ) {
        qCritical() << "No support for lzma decryption implemented, sorry.";
        return false;
    } else {
        qCritical() << "Blob contains no data";
        return false;
    }

    return true;
}

bool PbfParser::parseData()
{
    if ( !m_headerBlock.ParseFromArray( m_buffer.data(), m_buffer.size() ) ) {
        qCritical() << "failed to parse header block";
        return false;
    }

    for ( int i = 0; i < m_headerBlock.required_features_size(); ++i ) {
        string const & feature = m_headerBlock.required_features( i );
        if ( feature != "OsmSchema-V0.6" && feature != "DenseNodes" ) {
            qCritical() << "Support for feature " << feature.c_str() << "not implemented";
            return false;
        }
    }

    return true;
}

bool PbfParser::readNext()
{
    if ( !parseBlobHeader() )
        return false;

    if ( m_blobHeader.type() != "OSMData" ) {
        qCritical() << "invalid block type, found" << m_blobHeader.type().data() << "instead of OSMData";
        return false;
    }

    if ( !parseBlob() )
        return false;

    if ( !m_primitiveBlock.ParseFromArray( m_buffer.data(), m_buffer.size() ) ) {
        qCritical() << "failed to parse PrimitiveBlock";
        return false;
    }
    return true;
}

void PbfParser::loadGroup()
{
    const PrimitiveGroup& group = m_primitiveBlock.primitivegroup( m_currentGroup );
    if ( group.nodes_size() != 0 ) {
        m_mode = ModeNode;
    } else if ( group.ways_size() != 0 ) {
        m_mode = ModeWay;
    } else if ( group.relations_size() != 0 ) {
        m_mode = ModeRelation;
    } else if ( group.has_dense() )  {
        m_mode = ModeDense;
        m_lastDenseID = 0;
        m_lastDenseTag = 0;
        m_lastDenseLatitude = 0;
        m_lastDenseLongitude = 0;
        assert( group.dense().id_size() != 0 );
    } else
        assert( false );
}

void PbfParser::loadBlock()
{
    m_loadBlock = false;
    m_currentGroup = 0;
    m_currentEntity = 0;
}

void PbfParser::parseNode()
{
    const Node& inputNode = m_primitiveBlock.primitivegroup( m_currentGroup ).nodes( m_currentEntity );
    Marble::Node node;
    node.lat = ( ( double ) inputNode.lat() * m_primitiveBlock.granularity() + m_primitiveBlock.lat_offset() ) / ( 1000.0 * 1000.0 * 1000.0 );
    node.lon = ( ( double ) inputNode.lon() * m_primitiveBlock.granularity() + m_primitiveBlock.lon_offset() ) / ( 1000.0 * 1000.0 * 1000.0 );

    for ( int tag = 0; tag < inputNode.keys_size(); tag++ ) {

        QString key = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputNode.keys( tag ) ).data() );
        QString value = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputNode.vals( tag ) ).data() );

        if ( key == "name" ) {
            node.name = value;
        } else if ( key == "addr:street" ) {
            node.street = value;
            node.save = true;
        } else if ( key == "addr:housenumber" ) {
            node.houseNumber = value;
            node.save = true;
        } else if ( key == "addr:city" ) {
            node.city = value;
            node.save = true;
        } else {
            if ( shouldSave( Marble::NodeType, key, value ) ) {
                node.save = true;
            }
            setCategory( node, key, value );
        }
    }

    m_nodes[inputNode.id()] = node;

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).nodes_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}

void PbfParser::parseWay()
{
    const Way& inputWay = m_primitiveBlock.primitivegroup( m_currentGroup ).ways( m_currentEntity );
    Marble::Way way;

    for ( int tag = 0; tag < inputWay.keys_size(); tag++ ) {
        QString key = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputWay.keys( tag ) ).data() );
        QString value = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputWay.vals( tag ) ).data() );

        if ( key == "name" ) {
            way.name = value;
        } else if ( key == "addr:street" ) {
            way.street = value;
            way.save = true;
        } else if ( key == "addr:housenumber" ) {
            way.houseNumber = value;
            way.save = true;
        } else if ( key == "addr:city" ) {
            way.city = value;
            way.save = true;
        } else if ( key == "building" && value == "yes" ) {
            way.isBuilding = true;
        } else  {
            if ( shouldSave( Marble::WayType, key, value ) ) {
                way.save = true;
            }
            setCategory( way, key, value );
        }
    }

    long long lastRef = 0;
    for ( int i = 0; i < inputWay.refs_size(); i++ ) {
        lastRef += inputWay.refs( i );
        way.nodes.push_back( lastRef );
    }

    m_ways[inputWay.id()] = way;

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).ways_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}

void PbfParser::parseRelation()
{
    const Relation& inputRelation = m_primitiveBlock.primitivegroup( m_currentGroup ).relations( m_currentEntity );
    Marble::Relation relation;

    for ( int tag = 0; tag < inputRelation.keys_size(); tag++ ) {

        QString key = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputRelation.keys( tag ) ).data() );
        QString value = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputRelation.vals( tag ) ).data() );

        if ( key == "boundary" && value == "administrative" ) {
            relation.isAdministrativeBoundary = true;
        } else if ( key == "admin_level" ) {
            relation.adminLevel = value.toInt();
        } else if ( key == "name" ) {
            relation.name = value;
        } else if ( key == "type" && value == "multipolygon" ) {
            relation.isMultipolygon = true;
        }
    }

    long long lastRef = 0;
    for ( int i = 0; i < inputRelation.types_size(); i++ ) {
        //        RelationMember member;
        lastRef += inputRelation.memids( i );
        switch ( inputRelation.types( i ) ) {
        case OSMPBF::Relation::NODE:
            relation.nodes.push_back( lastRef );
            break;
        case OSMPBF::Relation::WAY: {
            string role = m_primitiveBlock.stringtable().s( inputRelation.roles_sid( i ) ).data();
            Marble::RelationRole relationRole = Marble::None;
            if ( role == "outer" ) relationRole = Marble::Outer;
            if ( role == "inner" ) relationRole = Marble::Inner;
            relation.ways.push_back( QPair<int, Marble::RelationRole>( lastRef, relationRole ) );
        }
        break;
        case OSMPBF::Relation::RELATION:
            relation.relations.push_back( lastRef );
        }
        //            member.ref = lastRef;
        //            member.role = m_primitiveBlock.stringtable().s( inputRelation.roles_sid( i ) ).data();
        //            relation->members.push_back( member );
    }

    m_relations[inputRelation.id()] = relation;

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).relations_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}

void PbfParser::parseDense()
{
    const DenseNodes& dense = m_primitiveBlock.primitivegroup( m_currentGroup ).dense();
    m_lastDenseID += dense.id( m_currentEntity );
    m_lastDenseLatitude += dense.lat( m_currentEntity );
    m_lastDenseLongitude += dense.lon( m_currentEntity );

    Marble::Node node;
    node.lat = ( ( double ) m_lastDenseLatitude * m_primitiveBlock.granularity() + m_primitiveBlock.lat_offset() ) / ( 1000.0 * 1000.0 * 1000.0 );
    node.lon = ( ( double ) m_lastDenseLongitude * m_primitiveBlock.granularity() + m_primitiveBlock.lon_offset() ) / ( 1000.0 * 1000.0 * 1000.0 );

    while ( true ) {
        if ( m_lastDenseTag >= dense.keys_vals_size() )
            break;

        int tagValue = dense.keys_vals( m_lastDenseTag );
        if ( tagValue == 0 ) {
            m_lastDenseTag++;
            break;
        }

        QString key = QString::fromUtf8( m_primitiveBlock.stringtable().s( dense.keys_vals( m_lastDenseTag ) ).data() );
        QString value = QString::fromUtf8( m_primitiveBlock.stringtable().s( dense.keys_vals( m_lastDenseTag + 1 ) ).data() );

        if ( key == "name" ) {
            node.name = value;
        } else if ( key == "addr:street" ) {
            node.street = value;
            node.save = true;
        } else if ( key == "addr:housenumber" ) {
            node.houseNumber = value;
            node.save = true;
        } else if ( key == "addr:city" ) {
            node.city = value;
            node.save = true;
        } else {
            if ( shouldSave( Marble::NodeType, key, value ) ) {
                node.save = true;
            }
            setCategory( node, key, value );
        }

        m_lastDenseTag += 2;
    }

    m_nodes[m_lastDenseID] = node;

    ++m_currentEntity;
    if ( m_currentEntity >= dense.id_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() ) {
            m_loadBlock = true;
        } else {
            loadGroup();
        }
    }
}
