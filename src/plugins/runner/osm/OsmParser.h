//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Thibaut Gridel <tgridel@free.fr>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2014      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
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
    static GeoDataDocument *createDocument(OsmNodes &nodes, OsmWays &way, OsmRelations &relations);
};

}

#endif // OSMPARSER_H
