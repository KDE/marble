#include "KMLLabelStyleParser.h"
#include "KMLLabelStyle.h"

KMLLabelStyleParser::KMLLabelStyleParser( KMLLabelStyle& labelStyle )
  : KMLColorStyleParser( labelStyle )
{
}

bool KMLLabelStyleParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return true;
}

bool KMLLabelStyleParser::endElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& qName )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return true;
}

bool KMLLabelStyleParser::characters( const QString& ch )
{
    if ( m_parsed ) {
        return false;
    }

    //TODO
    return true;
}
