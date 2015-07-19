//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMPLACEMARKTAGTRANSLATOR_H
#define MARBLE_OSMPLACEMARKTAGTRANSLATOR_H

#include "GeoTagWriter.h"
#include "OsmFeatureTagTranslator.h"

namespace Marble
{

class OsmPlacemarkTagTranslator : public OsmFeatureTagTranslator
{
public:
  OsmPlacemarkTagTranslator();

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const;
};

}

#endif

