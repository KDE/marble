//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015 Constantin Mihalache <mihalache.c94@gmail.com>

#include "KmlLabelStyleTagWriter.h"

#include "GeoDataLabelStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include <QDebug>

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(
        GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataLabelStyleType,
                                     kml::kmlTag_nameSpaceOgc22),
        new KmlLabelStyleTagWriter );

KmlLabelStyleTagWriter::KmlLabelStyleTagWriter() : KmlColorStyleTagWriter( kml::kmlTag_LabelStyle )
{
    //nothing to do here
}

bool KmlLabelStyleTagWriter::writeMid( const GeoNode *node, GeoWriter &writer ) const
{
    const GeoDataLabelStyle *style = static_cast<const GeoDataLabelStyle*>( node );

    writer.writeElement( kml::kmlTag_scale, QString::number(style->scale()));

    return true;
}

bool KmlLabelStyleTagWriter::isEmpty(const GeoNode *node) const
{
    const GeoDataLabelStyle *style = static_cast<const GeoDataLabelStyle*>( node );

    return style->scale() == 1.0;
}

QColor KmlLabelStyleTagWriter::defaultColor() const
{
    return QColor( Qt::black );
}

}
