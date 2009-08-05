//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef GEOWRITER_H
#define GEOWRITER_H

#include "GeoDataFeature.h"

#include <QtXml/QXmlStreamWriter>

namespace Marble{

/**
 * @brief Standard Marble way of writing XML
 * This class is intended to be a standardised way of writing XML for marble.
 * It works with the GeoData classes and writes XML based on the type of output
 * format that the writer is currently working with.
 */
class GeoWriter : public QXmlStreamWriter
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
     * @brief Set the current document type.
     * The current Document Type defines which set of hadlers are to be used
     * when writing the GeoDocument. This string should corrispond with the
     * string used to register the required Tag Writers in @see GeoTagWriter
     */
    void setDocumentType( const QString& documentType );
private:
    QString m_documentType;
};

}

#endif // GEOWRITER_H
