//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLPLACEMARKPARSER_H
#define KMLPLACEMARKPARSER_H

#include "KMLFeatureParser.h"
#include "KMLObjectParser.h"

class KMLObjectParser;
class KMLPlaceMark;

class KMLPlaceMarkParser : public KMLFeatureParser
{
 public:
    KMLPlaceMarkParser( KMLPlaceMark& placemark );
    virtual ~KMLPlaceMarkParser();

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );

 private:
    KMLObjectParser*    m_currentParser;
};

#endif // KMLPLACEMARKPARSER_H
