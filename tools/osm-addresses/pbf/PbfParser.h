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

#ifndef PBFPARSER_H
#define PBFPARSER_H

#include "fileformat.pb.h"
#include "osmformat.pb.h"
#include "../OsmParser.h"

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtCore/QDataStream>

class PbfParser : public Marble::OsmParser
{
public:
    PbfParser();

    virtual bool parse( const QFileInfo &file );

private:
    enum Mode {
        ModeNode,
        ModeWay,
        ModeRelation,
        ModeDense
    };

    bool parseBlobHeader();

    bool parseBlob();

    bool parseData();

    bool readNext();

    void loadBlock();

    void loadGroup();

    void parseNode();

    void parseWay();

    void parseRelation();

    void parseDense();

    QByteArray m_buffer;

    QDataStream m_stream;

    OSMPBF::BlobHeader m_blobHeader;

    OSMPBF::Blob m_blob;

    OSMPBF::HeaderBlock m_headerBlock;

    OSMPBF::PrimitiveBlock m_primitiveBlock;

    Mode m_mode;

    int m_currentGroup;

    int m_currentEntity;

    bool m_loadBlock;

    long long m_lastDenseID;
    long long m_lastDenseLatitude;
    long long m_lastDenseLongitude;
    int m_lastDenseTag;

    std::vector< int > m_nodeTagIDs;
    std::vector< int > m_wayTagIDs;
    std::vector< int > m_relationTagIDs;

    QHash< QString, int > m_nodeTags;
    QHash< QString, int > m_wayTags;
    QHash< QString, int > m_relationTags;
};

#endif // PBFPARSER_H
