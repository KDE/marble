// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoWriter.h"

#include "GeoDocument.h"
#include "GeoTagWriter.h"
#include "KmlElementDictionary.h"

#include "MarbleDebug.h"

namespace Marble
{

GeoWriter::GeoWriter()
{
    // FIXME: work out a standard way to do this.
    m_documentType = QString::fromLatin1(kml::kmlTag_nameSpaceOgc22);
}

bool GeoWriter::write(QIODevice *device, const GeoNode *feature)
{
    setDevice(device);
    setAutoFormatting(true);
    writeStartDocument();

    // FIXME: write the starting tags. Possibly register a tag handler to do this
    //  with a null string as the object name?

    GeoTagWriter::QualifiedName name({}, m_documentType);
    const GeoTagWriter *writer = GeoTagWriter::recognizes(name);
    if (writer) {
        // FIXME is this too much of a hack?
        writer->write(/* node = */ nullptr, *this); // node is never used in write()
    } else {
        mDebug() << "There is no GeoWriter registered for: " << name;
        return false;
    }

    if (!writeElement(feature)) {
        return false;
    }

    // close the document
    writeEndElement();
    return true;
}

bool GeoWriter::writeElement(const GeoNode *object)
{
    // Add checks to see that everything is ok here

    GeoTagWriter::QualifiedName name(QString::fromLatin1(object->nodeType()), m_documentType);
    const GeoTagWriter *writer = GeoTagWriter::recognizes(name);

    if (writer) {
        if (!writer->write(object, *this)) {
            mDebug() << "An error has been reported by the GeoWriter for: " << name;
            return false;
        }
    } else {
        mDebug() << "There is no GeoWriter registered for: " << name;
        return true;
    }
    return true;
}

void GeoWriter::setDocumentType(const QString &documentType)
{
    m_documentType = documentType;
}

void GeoWriter::writeElement(const QString &namespaceUri, const QString &key, const QString &value)
{
    writeStartElement(namespaceUri, key);
    writeCharacters(value);
    writeEndElement();
}

void GeoWriter::writeElement(const QString &key, const QString &value)
{
    writeStartElement(key);
    writeCharacters(value);
    writeEndElement();
}

void GeoWriter::writeOptionalElement(const QString &key, const QString &value, const QString &defaultValue)
{
    if (value != defaultValue) {
        writeElement(key, value);
    }
}

void GeoWriter::writeOptionalAttribute(const QString &key, const QString &value, const QString &defaultValue)
{
    if (value != defaultValue) {
        writeAttribute(key, value);
    }
}

}
