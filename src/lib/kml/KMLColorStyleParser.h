//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLCOLORSTYLEPARSER_H
#define KMLCOLORSTYLEPARSER_H

#include "KMLObjectParser.h"

class KMLColorStyle;

class KMLColorStyleParser : public KMLObjectParser
{
  public:
    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );

  protected:
    KMLColorStyleParser( KMLColorStyle& style );
};

#endif
