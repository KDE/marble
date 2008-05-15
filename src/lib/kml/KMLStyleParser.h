//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLSTYLEPARSER_H
#define KMLSTYLEPARSER_H

#include "KMLObjectParser.h"

class KMLStyle;

class KMLStyleParser : public KMLObjectParser
{
  public:
    explicit KMLStyleParser( KMLStyle& style );
    virtual ~KMLStyleParser();

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );

  private:
    KMLObjectParser* m_currentParser;
};

#endif
