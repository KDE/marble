//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_GEOSCENEGEODATA_H
#define MARBLE_GEOSCENEGEODATA_H

#include <geodata_export.h>
#include "GeoDocument.h"
#include "GeoSceneLayer.h"

namespace Marble
{

class GEODATA_EXPORT GeoSceneGeodata : public GeoSceneAbstractDataset
{
 public:
    GeoSceneGeodata( QString name );
    virtual ~GeoSceneGeodata();
    virtual const char* nodeType() const;
     
    QString name() const;
     
    QString sourceFile() const;
    void setSourceFile( QString sourceFile );
     
    QString sourceFileFormat() const;
    void setSourceFileFormat( QString format );

    virtual QString type();
    
 private:
    QString m_name;
    QString m_sourceFile;
    QString m_sourceFileFormat;
};

}

#endif
