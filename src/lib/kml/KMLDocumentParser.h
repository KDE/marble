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

#include <QtXml/QXmlDefaultHandler>

class KMLDocument;

class KMLDocumentParser : public QXmlDefaultHandler
{
 public:
    KMLDocumentParser( KMLDocument& document );

 private:
    KMLDocument& m_document;
};

#endif // KMLDOCUMENTPARSER_H
