//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOWRITER_H
#define MARBLE_GEOWRITER_H

#include "GeoDataFeature.h"
#include "marble_export.h"

#include <QtXml/QXmlStreamWriter>

namespace Marble
{

/**
 * @brief Standard Marble way of writing XML
 * This class is intended to be a standardised way of writing XML for marble.
 * It works with the GeoData classes and writes XML based on the type of output
 * format that the writer is currently working with.
 */
class MARBLE_EXPORT GeoWriter : public QXmlStreamWriter
{
public:
    GeoWriter();

    /**
     * @brief The main API call to use the XML writer.
     * To use the XML writer you need to provide an IODevice to write the XML to
     * and a QList of GeoDataFeatures which contains the data you wish to write.
     * To define the type of XML document that is to be written you need to set
     * the current Document Type for this GeoWriter. See @see setDocumentType()
     */
    bool write( QIODevice* device,
                const QList<GeoDataFeature>& features);


    /**
     * @brief Overloaded convenience method to write a single feature or tree
     * This convenience method allows you to call the GeoWriter using just a
     * single @see GeoDataFeature. This accounts for the case where a KML
     * document is contained in one single root element.
     * See @see write(QIODevice*, const QList<GeoDataFeature>&) for more details
     */
    bool write( QIODevice* device, const GeoDataFeature& feature );

    /**
     * @brief Set the current document type.
     * The current Document Type defines which set of handlers are to be used
     * when writing the GeoDocument. This string should correspond with the
     * string used to register the required Tag Writers in @see GeoTagWriter
     */
    void setDocumentType( const QString& documentType );

    /**
     * @brief Convenience method to write <key>value</key>
     *
     **/
    void writeElement( const QString &key, const QString &value );

    /**
     * @brief Convenience method to write <key>value</key> if value is
     *   not empty. Otherwise, nothing is written.
     *
     **/
    void writeOptionalElement( const QString &key, const QString &value );

private:
    friend class GeoTagWriter;
    bool writeElement( const GeoDataObject& object );

private:
    QString m_documentType;
};

}

#endif
