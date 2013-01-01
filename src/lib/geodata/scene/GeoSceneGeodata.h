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

#include <QtGui/QPen>
#include <QtGui/QBrush>

namespace Marble
{

class GEODATA_EXPORT GeoSceneGeodata : public GeoSceneAbstractDataset
{
 public:
    GeoSceneGeodata( QString name );
    virtual ~GeoSceneGeodata();
    virtual const char* nodeType() const;
     
    QString property() const;
    void setProperty( QString property );
     
    QString sourceFile() const;
    void setSourceFile( QString sourceFile );
     
    QPen pen() const;
    void setPen( const QPen& pen );

    QBrush brush() const;
    void setBrush( const QBrush& brush );

    virtual QString type();
    
 private:
    QString m_property;
    QString m_sourceFile;
    QPen    m_pen;
    QBrush  m_brush;
};

}

#endif
