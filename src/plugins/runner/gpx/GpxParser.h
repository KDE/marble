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
    virtual ~GpxParser();

private:
    virtual bool isValidElement(const QString& tagName) const;
    virtual bool isValidRootElement();
    virtual void raiseRootElementError();

    virtual GeoDocument* createDocument() const;

};

}

#endif // GPXPARSER_H
