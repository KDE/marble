//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef GPXPARSER_H
#define GPXPARSER_H

#include "GeoParser.h"

namespace Marble {

class GpxParser : public GeoParser
{
public:
    GpxParser();
    ~GpxParser() override;

private:
    bool isValidElement(const QString& tagName) const override;
    bool isValidRootElement() override;

    GeoDocument* createDocument() const override;

};

}

#endif // GPXPARSER_H
