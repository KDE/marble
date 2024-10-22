// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Constantin Mihalache <mihalache.c94@gmail.com>

#include "KmlLabelStyleTagWriter.h"

#include "GeoDataLabelStyle.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include <QDebug>

namespace Marble
{

static GeoTagWriterRegistrar s_writerLineStyle(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataLabelStyleType),
                                                                           QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                               new KmlLabelStyleTagWriter);

KmlLabelStyleTagWriter::KmlLabelStyleTagWriter()
    : KmlColorStyleTagWriter(QString::fromLatin1(kml::kmlTag_LabelStyle))
{
    // nothing to do here
}

bool KmlLabelStyleTagWriter::writeMid(const GeoNode *node, GeoWriter &writer) const
{
    const auto style = static_cast<const GeoDataLabelStyle *>(node);

    writer.writeElement(QString::fromLatin1(kml::kmlTag_scale), QString::number(style->scale()));

    return true;
}

bool KmlLabelStyleTagWriter::isEmpty(const GeoNode *node) const
{
    const auto style = static_cast<const GeoDataLabelStyle *>(node);

    return style->scale() == 1.0;
}

QColor KmlLabelStyleTagWriter::defaultColor() const
{
    return {Qt::black};
}

}
