/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_JSONPARSER_H
#define MARBLE_JSONPARSER_H

class QIODevice;

namespace Marble {

class GeoDataDocument;

class JsonParser
{
public:
    JsonParser();
    ~JsonParser();

    /**
     * @brief parse the json file
     * @return true if the parsed has been successful
     */
    bool read(QIODevice*);

    /**
     * @brief retrieve the parsed document and reset the parser
     * If parsing was successful, retrieve the resulting document
     * and set the contained m_document pointer to 0.
     */
    GeoDataDocument* releaseDocument();

private:

    GeoDataDocument* m_document;
};

}

#endif // MARBLE_JSONPARSER_H
