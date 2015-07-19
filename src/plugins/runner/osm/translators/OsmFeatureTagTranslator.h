//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMFEATURETAGTRANSLATOR_H
#define MARBLE_OSMFEATURETAGTRANSLATOR_H

#include "GeoTagWriter.h"

#include <QString>

namespace Marble
{

// No registration needed for this class, as it is abstract.

class OsmFeatureTagTranslator: public GeoTagWriter
{
public:
    explicit OsmFeatureTagTranslator();

    bool write( const GeoNode *node, GeoWriter& writer ) const;

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const = 0;
};

}

#endif

