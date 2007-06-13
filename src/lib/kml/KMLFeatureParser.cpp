#include "KMLFeatureParser.h"

KMLFeatureParser::KMLFeatureParser()
{
}

bool KMLFeatureParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString &name,
                            const QXmlAttributes& atts )
{
    //TODO
    return true;
}

bool KMLFeatureParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString &name )
{
    //TODO
    return false;
}

bool KMLFeatureParser::characters( const QString& str )
{
    //TODO
    return false;
}
