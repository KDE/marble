//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLCONTAINERPARSER_H
#define KMLCONTAINERPARSER_H

#include "KMLFeatureParser.h"

class KMLContainer;
class KMLPlaceMark;

class KMLContainerParser : public KMLFeatureParser
{
 public:
    virtual ~KMLContainerParser();

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );

 protected:
    KMLContainerParser( KMLContainer& container );

 private:
    KMLObjectParser*      m_currentParser;
    KMLPlaceMark*         m_currentPlaceMark;
};

#endif // KMLCONTAINERPARSER_H
