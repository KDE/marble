// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#include "KmlSimpleFieldTagWriter.h"

#include <QString>

#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"

namespace Marble
{

static GeoTagWriterRegistrar s_writerSimpleField(GeoTagWriter::QualifiedName(QString::fromLatin1(GeoDataTypes::GeoDataSimpleFieldType),
                                                                             QString::fromLatin1(kml::kmlTag_nameSpaceOgc22)),
                                                 new KmlSimpleFieldTagWriter);

bool KmlSimpleFieldTagWriter::write(const GeoNode *node, GeoWriter &writer) const
{
    const auto simpleField = static_cast<const GeoDataSimpleField *>(node);
    writer.writeStartElement(QString::fromUtf8(kml::kmlTag_SimpleField));
    writer.writeAttribute(QStringLiteral("name"), simpleField->name());
    GeoDataSimpleField::SimpleFieldType simpleFieldType = simpleField->type();
    QString type = resolveType(simpleFieldType);
    writer.writeAttribute("type", type);

    QString displayName = simpleField->displayName();
    writer.writeElement(QStringLiteral("displayName"), displayName);

    writer.writeEndElement();

    return true;
}

QString KmlSimpleFieldTagWriter::resolveType(GeoDataSimpleField::SimpleFieldType type)
{
    switch (type) {
    case GeoDataSimpleField::String:
        return QStringLiteral("string");
    case GeoDataSimpleField::Int:
        return QStringLiteral("int");
    case GeoDataSimpleField::UInt:
        return QStringLiteral("uint");
    case GeoDataSimpleField::Short:
        return QStringLiteral("short");
    case GeoDataSimpleField::UShort:
        return QStringLiteral("ushort");
    case GeoDataSimpleField::Float:
        return QStringLiteral("float");
    case GeoDataSimpleField::Double:
        return QStringLiteral("double");
    case GeoDataSimpleField::Bool:
        return QStringLiteral("bool");
    }

    Q_ASSERT(false);
    return QStringLiteral("string");
}

}
