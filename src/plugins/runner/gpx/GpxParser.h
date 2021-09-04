//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

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
