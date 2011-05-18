//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_XMLPARSER_H
#define MARBLE_XMLPARSER_H

#include "../OsmParser.h"

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
#include <QtXml/QXmlDefaultHandler>

namespace Marble
{

class XmlParser : public OsmParser, private QXmlDefaultHandler
{
    Q_OBJECT
public:
    explicit XmlParser( QObject *parent = 0 );

protected:
    virtual bool parse( const QFileInfo &file );

private:
    virtual bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );

    virtual bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName );

    Node m_node;

    Way m_way;

    Relation m_relation;

    int m_id;

    ElementType m_element;

};

}

#endif // MARBLE_XMLPARSER_H
