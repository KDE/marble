/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOPARSER_H
#define MARBLE_GEOPARSER_H

#include <QPair>
#include <QStack>
#include <QXmlStreamReader>

#include "geodata_export.h"

namespace Marble
{

using GeoDataGenericSourceType = int;

class GeoDocument;
class GeoNode;
class GeoStackItem;

class GEODATA_EXPORT GeoParser : public QXmlStreamReader
{
public:
    using QualifiedName = QPair<QString, QString>; // Tag Name & Namespace pair

    explicit GeoParser(GeoDataGenericSourceType sourceType);
    virtual ~GeoParser();

    /**
     * @brief Main API for reading the XML document.
     * This is the only method that is necessary to call to start the GeoParser.
     * To retrieve the resulting data see @see releaseDocument() and
     * @see releaseModel()
     */
    bool read(QIODevice *);

    /**
     * @brief retrieve the parsed document and reset the parser
     * If parsing was successful, retrieve the resulting document
     * and set the contained m_document pointer to 0.
     */
    GeoDocument *releaseDocument();
    GeoDocument *activeDocument()
    {
        return m_document;
    }

    // Used by tag handlers, to be overridden by GeoDataParser/GeoSceneParser
    virtual bool isValidElement(const QString &tagName) const;

    // Used by tag handlers, to access a parent element's associated GeoStackItem
    GeoStackItem parentElement(unsigned int depth = 0) const;

    // Used by tag handlers, to emit a warning while parsing
    void raiseWarning(const QString &);

    // Used by tag handlers, to retrieve the value for an attribute of the currently parsed element
    QString attribute(const char *attributeName) const;

protected:
    /**
     * This method is intended to check if the current element being served by
     * the GeoParser is a valid Document Root element. This method is to be
     * implemented by GeoDataParser/GeoSceneParser and must check based on the
     * current XML Document type, e.g. KML, GPX etc.
     * @return @c true if the element is a valid document root.
     */
    virtual bool isValidRootElement() = 0;

    virtual GeoDocument *createDocument() const = 0;

protected:
    GeoDocument *m_document;
    GeoDataGenericSourceType m_source;

private:
    void parseDocument();
    QStack<GeoStackItem> m_nodeStack;
};

class GeoStackItem
{
public:
    GeoStackItem()
        : m_qualifiedName()
        , m_node(nullptr)
    {
    }

    GeoStackItem(const GeoParser::QualifiedName &qualifiedName, GeoNode *node)
        : m_qualifiedName(qualifiedName)
        , m_node(node)
    {
    }

    // Fast path for tag handlers
    bool represents(const char *tagName) const
    {
        return m_node && QLatin1StringView(tagName) == m_qualifiedName.first;
    }

    // Helper for tag handlers. Does NOT guard against miscasting. Use with care.
    template<class T>
    T *nodeAs()
    {
        Q_ASSERT(dynamic_cast<T *>(m_node) != nullptr);
        return static_cast<T *>(m_node);
    }

    template<class T>
    bool is() const
    {
        return nullptr != dynamic_cast<T *>(m_node);
    }

    GeoParser::QualifiedName qualifiedName() const
    {
        return m_qualifiedName;
    }
    GeoNode *associatedNode() const
    {
        return m_node;
    }

private:
    friend class GeoParser;
    void assignNode(GeoNode *node)
    {
        m_node = node;
    }
    GeoParser::QualifiedName m_qualifiedName;
    GeoNode *m_node = nullptr;
};

}

#endif
