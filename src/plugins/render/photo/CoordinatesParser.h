//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef COORDINATESPARSER_H
#define COORDINATESPARSER_H

#include <QXmlStreamReader>

class QIODevice;

namespace Marble
{
    
class GeoDataCoordinates;

class CoordinatesParser : public QXmlStreamReader
{
public:
    explicit CoordinatesParser( GeoDataCoordinates *coordinates );

    bool read( QIODevice *device );

private:
    void readUnknownElement();
    
    void readRsp();
    
    void readPhoto();
    
    void readLocation();
    
    GeoDataCoordinates *m_coordinates;
};

} // Marble namespace

#endif // COORDINATESPARSER_H

