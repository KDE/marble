/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "GeoDataDocument.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>

namespace Marble {

class JsonParser
{
public:
    JsonParser();
    virtual ~JsonParser();

    /**
     * @brief parse the json file
     * @return true if the parsed has been succesful
     */
    bool read(QIODevice*);

    /**
     * @brief retrieve the parsed document and reset the parser
     * If parsing was successful, retrieve the resulting document
     * and set the contained m_document pointer to 0.
     */
    GeoDataDocument* releaseDocument();

private:

    GeoDataDocument* createDocument() const;

    GeoDataDocument* m_document;

    // json parsing engine
    QScriptValue m_data;
    QScriptEngine m_engine;

};

}

#endif // JSONPARSER_H
