//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// SPDX-FileCopyrightText: 2014 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef OSMPARSER_H
#define OSMPARSER_H

#include "OsmNode.h"
#include "OsmWay.h"
#include "OsmRelation.h"

#include <QString>

namespace Marble {

class GeoDataDocument;

class OsmParser
{
public:
    static GeoDataDocument* parse(const QString &filename, QString &error);

private:
    static GeoDataDocument* parseXml(const QString &filename, QString &error);
    static GeoDataDocument* parseO5m(const QString &filename, QString &error);
    static GeoDataDocument* parseOsmPbf(const QString &filename, QString &error);
    static GeoDataDocument *createDocument(OsmNodes &nodes, OsmWays &way, OsmRelations &relations);
};

}

#endif // OSMPARSER_H
