// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
//

#ifndef MARBLE_GEOTAGWRITER_H
#define MARBLE_GEOTAGWRITER_H

#include <QHash>
#include <QPair>

#include <marble_export.h>

namespace Marble
{

class GeoNode;
class GeoWriter;

/**
 * @brief Base class intended to be subclassed by specific XML tag writers
 * This class provides a base class that allows the writing of many different
 * XML formats. The system used to implement this writing system is very strongly
 * based on the @see GeoTagHandler system.
 */
class MARBLE_EXPORT GeoTagWriter
{
public:
    virtual bool write(const GeoNode *node, GeoWriter &writer) const = 0;

    /**
     * @brief Object Name and Namespace Pair
     * This type is intended to be used in a similar way to @see GeoParser::QualifiedName
     * but in practice will act differently. The Namespace will not be an XML
     * namespace directly but instead it will refere to a Document Type so that
     * the GeoWriter will be able to identify what GeoTagWriter to use even in
     * absence of an XML namespace. This also allows for the case where data
     * using an internal representation of the KML classes can be outputted in
     * alternative XML formats. For XML formats that have namespaces this
     * document type will usually correspond with the XML namespace. Use in the
     * order QPair<QString tagName, QString documentType>.
     */
    using QualifiedName = QPair<QString, QString>;

protected:
    GeoTagWriter();
    virtual ~GeoTagWriter();

    static bool writeElement(const GeoNode *object, GeoWriter &writer);

private:
    // Only our registrar is allowed to register tag writers.
    friend struct GeoTagWriterRegistrar;
    static void registerWriter(const QualifiedName &, const GeoTagWriter *);
    static void unregisterWriter(const QualifiedName &);

private:
    // Collect the Tag Writers and provide a singleton like accessor
    using TagHash = QHash<QualifiedName, const GeoTagWriter *>;
    static TagHash *tagWriterHash();

private:
    // Only our writer is allowed to access tag handlers.
    friend class GeoWriter;
    friend class GeoDataDocumentWriter;
    static const GeoTagWriter *recognizes(const QualifiedName &);
};

// Helper structure
struct GeoTagWriterRegistrar {
public:
    GeoTagWriterRegistrar(const GeoTagWriter::QualifiedName &name, const GeoTagWriter *writer)
        : m_name(name)
    {
        GeoTagWriter::registerWriter(name, writer);
    }

    ~GeoTagWriterRegistrar()
    {
        GeoTagWriter::unregisterWriter(m_name);
    }

private:
    GeoTagWriter::QualifiedName m_name;
};

}

#endif
