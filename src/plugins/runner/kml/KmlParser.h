//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef KMLPARSER_H
#define KMLPARSER_H

#include "GeoParser.h"
#include "KmlDocument.h"

namespace Marble {

class KmlParser : public GeoParser
{
public:
    KmlParser();
    virtual ~KmlParser();

private:
    virtual bool isValidElement(const QString& tagName) const;
    virtual bool isValidRootElement();

    virtual GeoDocument* createDocument() const;

};

}

#endif // KMLPARSER_H
