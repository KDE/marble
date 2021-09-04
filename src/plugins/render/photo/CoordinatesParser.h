// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
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

