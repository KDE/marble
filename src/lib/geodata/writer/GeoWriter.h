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

#include <QtXml/QXmlStreamWriter>

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
};

#endif // GEOWRITER_H
