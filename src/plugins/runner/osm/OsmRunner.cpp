//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "OsmRunner.h"

#include "GeoDataDocument.h"
#include "OsmParser.h"
#include "MarbleDebug.h"

namespace Marble
{

OsmRunner::OsmRunner(QObject *parent) :
    ParsingRunner(parent)
{
}

GeoDataDocument *OsmRunner::parseFile(const QString &fileName, DocumentRole role, QString &error)
{
    GeoDataDocument* document = OsmParser::parse(fileName, error);
    if (document) {
        document->setDocumentRole(role);
        document->setFileName(fileName);
    }
    return document;
}

}

#include "moc_OsmRunner.cpp"
