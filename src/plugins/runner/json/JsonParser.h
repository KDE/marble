/*
    This file is part of the Marble Virtual Globe.

    This program is free software licensed under the GNU LGPL. You can
    find a copy of this license in LICENSE.txt in the top directory of
    the source code.

    Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
    Copyright 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#ifndef MARBLE_JSONPARSER_H
#define MARBLE_JSONPARSER_H

class QIODevice;
class QJsonObject;

#include <QVector>

namespace Marble {

class GeoDataDocument;
class GeoDataGeometry;
class GeoDataIconStyle;
class GeoDataLineStyle;
class GeoDataPolyStyle;
class GeoDataLabelStyle;

class JsonParser
{
public:
    JsonParser();
    ~JsonParser();

    /**
     * @brief parse the GeoJSON file
     * @return true if parsing of the file was successful
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

    GeoDataIconStyle*  m_iconStylePoints;
    GeoDataIconStyle*  m_iconStyleOther;
    GeoDataLineStyle*  m_lineStyle;
    GeoDataPolyStyle*  m_polyStyle;
    GeoDataLabelStyle* m_labelStyle;

    /**
     * @brief parse a top-level GeoJSON object (FeatureCollection or Feature)
     * @param jsonObject  the object to parse
     * @return true if parsing of the top-level object was successful
     */
    bool parseGeoJsonTopLevel(const QJsonObject&);

    /**
      * @brief parse a sub-level GeoJSON object
      * @param jsonObject    the object to parse
      * @param geometryList  a list of geometries pass back to the caller
      * @param hasPoints     a boolean passed back to the caller: true if Points exist in geometry
      * @return true if parsing of the object was successful
      */
    bool parseGeoJsonSubLevel(const QJsonObject&, QVector<GeoDataGeometry*>&, bool&);
};

}

#endif // MARBLE_JSONPARSER_H
