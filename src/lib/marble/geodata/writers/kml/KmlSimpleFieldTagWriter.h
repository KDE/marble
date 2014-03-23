//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_KMLSIMPLEFIELDTAGWRITER_H
#define MARBLE_KMLSIMPLEFIELDTAGWRITER_H

#include "GeoTagWriter.h"
#include "GeoDataSimpleField.h"

class QString;

namespace Marble
{

class KmlSimpleFieldTagWriter : public GeoTagWriter
{
  public:
    virtual bool write( const GeoNode *node, GeoWriter& writer ) const;

private:
    static QString resolveType( GeoDataSimpleField::SimpleFieldType type );
};

}

#endif
