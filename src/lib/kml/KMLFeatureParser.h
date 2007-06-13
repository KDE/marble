#ifndef KMLFEATUREPARSER_H
#define KMLFEATUREPARSER_H

#include "KMLObjectParser.h"

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
    KMLFeatureParser();
};

#endif // KMLFEATUREPARSER_H
