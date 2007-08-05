//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLFEATUREPARSER_H
#define KMLFEATUREPARSER_H

#include "KMLObjectParser.h"

class KMLFeature;

class KMLFeatureParser : public KMLObjectParser
{
 public:
    virtual bool startElement( const QString& namespaceURI,
                               const QString& localName,
                               const QString &name,
                               const QXmlAttributes& atts );

    virtual bool endElement( const QString& namespaceURI,
                             const QString& localName,
                             const QString &name );

    virtual bool characters( const QString& str );

 protected:
    KMLFeatureParser( KMLFeature& feature );

 private:
    enum
    {
        IDLE,
        WAIT_NAME,
        WAIT_DESCRIPTION,
        WAIT_ADDRESS,
        WAIT_PHONE_NUMBER,
        WAIT_VISIBILITY
    };

    int m_phase;
};

#endif // KMLFEATUREPARSER_H
