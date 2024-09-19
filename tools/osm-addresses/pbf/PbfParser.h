// SPDX-License-Identifier: GPL-3.0-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//
// This file originates from the MoNav project where it was named pbfreader.h and
// SPDX-FileCopyrightText: 2010 Christian Vetter veaac.fdirct @gmail.com
//

#ifndef PBFPARSER_H
#define PBFPARSER_H

#include "../OsmParser.h"
#include "fileformat.pb.h"
#include "osmformat.pb.h"

#include <QDataStream>
#include <QFile>
#include <QSet>
#include <QString>

class PbfParser : public Marble::OsmParser
{
public:
    PbfParser();

    bool parse(const QFileInfo &file, int pass, bool &needAnotherPass) override;

private:
    enum Mode { ModeNode, ModeWay, ModeRelation, ModeDense };

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
    int m_pass;

    QSet<int> m_referencedWays;
    QSet<int> m_referencedNodes;
};

#endif // PBFPARSER_H
