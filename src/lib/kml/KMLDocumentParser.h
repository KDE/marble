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

#include <QtCore/QStack>
#include <QtXml/QXmlDefaultHandler>

class KMLDocument;
class KMLObjectParser;

class KMLDocumentParser : public QXmlDefaultHandler
{
 public:
    KMLDocumentParser( KMLDocument& document );

    void switchCurrentParser( KMLObjectParser* parser );

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

 private:
    KMLDocument& m_document;
    KMLObjectParser* m_currentParser;

    QStack <KMLObjectParser*> m_parserStack;
};

#endif // KMLDOCUMENTPARSER_H
