//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "GeoSceneGeodata.h"
#include "GeoSceneTypes.h"

namespace Marble
{
GeoSceneGeodata::GeoSceneGeodata( QString name )
    : GeoSceneAbstractDataset( name ),
      m_name( name ),
      m_sourceFile( QString() ),
      m_sourceFileFormat( QString() )
{
}

GeoSceneGeodata::~GeoSceneGeodata()
{
}

const char* GeoSceneGeodata::nodeType() const
{
    return GeoSceneTypes::GeoSceneGeodataType;
}

QString GeoSceneGeodata::name() const
{
    return m_name;
}

QString GeoSceneGeodata::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneGeodata::setSourceFile(QString sourceFile)
{
    m_sourceFile = sourceFile;
}

QString GeoSceneGeodata::sourceFileFormat() const
{
    return m_sourceFileFormat;
}

void GeoSceneGeodata::setSourceFileFormat(QString format)
{
    m_sourceFileFormat = format;
}

QString GeoSceneGeodata::type()
{
    return "geodata";
}

}

#include "GeoSceneGeodata.moc"
