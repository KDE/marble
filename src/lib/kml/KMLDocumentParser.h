//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#ifndef KMLDOCUMENTPARSER_H
#define KMLDOCUMENTPARSER_H

#include "KMLContainerParser.h"

class KMLDocument;

class KMLDocumentParser : public KMLContainerParser
{
 public:
    KMLDocumentParser( KMLDocument& document );

    /*
     * QXmlDefaultHandler virtual methods
     */
    virtual bool startDocument();
    virtual bool endDocument();

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );
};

#endif // KMLDOCUMENTPARSER_H
