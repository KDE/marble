//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMDOCUMENTTAGWRITER_H
#define MARBLE_OSMDOCUMENTTAGWRITER_H

#include "GeoTagWriter.h"
#include "OsmFeatureTagTranslator.h"

namespace Marble
{
/**
     This helper class organizes a GeoDataDocument to
     meet the following OSM standard structure:

     <xml>
     <bounds>

     block of nodes: ( <node>...) 1st
     block of ways:  ( <way> ...) 2nd
     block of relations: ( <relation> ...) 3rd

     See http://wiki.openstreetmap.org/wiki/OSM_XML#Contents

*/
class GeoDataLineString;
class OsmPlacemarkData;

class OsmDocumentTagTranslator : public OsmFeatureTagTranslator
{
public:
  OsmDocumentTagTranslator();

protected:
    virtual bool writeMid( const GeoNode *node, GeoWriter& writer ) const;

private:
  typedef QPair<const GeoDataLineString*, OsmPlacemarkData > OsmBound;

};

}

#endif

