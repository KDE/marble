//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLICONSTYLEPARSER_H
#define KMLICONSTYLEPARSER_H

#include "KMLColorStyleParser.h"

class QString;
class KMLIconStyle;

class KMLIconStyleParser : public KMLColorStyleParser
{
  public:
    explicit KMLIconStyleParser( KMLIconStyle& iconStyle );

    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString& name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString& qName );

    virtual bool characters( const QString& ch );

  private:
    void loadIcon( const QString& hrefValue );

  private:
    enum
    {
        IDLE,
        WAIT_ICON,
        WAIT_HREF
    };

    int m_phase;
};

#endif
