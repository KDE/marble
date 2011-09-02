/*
    Copyright (C) 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEOPARSER_H
#define MARBLE_GEOPARSER_H

#include <QtCore/QStack>
#include <QtXml/QXmlStreamReader>

#include <geodata_export.h>
#include "GeoTagHandler.h"

namespace Marble
{

typedef int GeoDataGenericSourceType;

class GeoDocument;
class GeoGraphicsItem;
class GeoStackItem;

class GEODATA_EXPORT GeoParser : public QXmlStreamReader
{
 public:
    explicit GeoParser( GeoDataGenericSourceType sourceType );
    virtual ~GeoParser();

    /**
     * @brief Main API for reading the XML document.
     * This is the only method that is necessary to call to start the GeoParser.
     * To retrieve the resulting data see @see releaseDocument() and
     * @see releaseModel()
     */
    bool read( QIODevice* );

    /**
     * @brief retrieve the parsed document and reset the parser
     * If parsing was successful, retrieve the resulting document
     * and set the contained m_document pointer to 0.
     */
    GeoDocument* releaseDocument();
    GeoDocument* activeDocument() { return m_document; }

    QList<GeoGraphicsItem*>* releaseModel();
    QList<GeoGraphicsItem*>* activeModel();

    // Used by tag handlers, to be overridden by GeoDataParser/GeoSceneParser
    virtual bool isValidElement( const QString& tagName ) const;

    // Used by tag handlers, to access a parent element's associated GeoStackItem
    GeoStackItem parentElement( unsigned int depth = 0 ) const;

    // Used by tag handlers, to emit a warning while parsing
    void raiseWarning( const QString& );

    // Used by tag handlers, to retrieve the value for an attribute of the currently parsed element
    QString attribute( const char* attributeName ) const;

protected:
    /**
     * This method is intended to check if the current element being served by
     * the GeoParser is a valid Document Root element. This method is to be
     * implemented by GeoDataParser/GeoSceneParser and must check based on the
     * current XML Document type, e.g. KML, GPX etc.
     * @return @c true if the element is a valid document root.
     */
    virtual bool isValidRootElement() = 0;

    virtual void raiseRootElementError();

    virtual GeoDocument* createDocument() const = 0;

protected:
    GeoDocument* m_document;
    QList<GeoGraphicsItem*>* m_itemModel;
    GeoDataGenericSourceType m_source;

private:
    void parseDocument();
    QStack<GeoStackItem> m_nodeStack;
};

class GeoStackItem : public QPair<GeoTagHandler::QualifiedName, GeoNode*>
{
 public:
    GeoStackItem()
        : QPair<GeoTagHandler::QualifiedName, GeoNode*>()
    {
    }

    GeoStackItem( const GeoTagHandler::QualifiedName& qName, GeoNode* node )
        : QPair<GeoTagHandler::QualifiedName, GeoNode*>( qName, node )
    {
    }

    // Fast path for tag handlers
    bool represents( const char* tagName ) const
    {
        return second && tagName == first.first;
    }

    // Helper for tag handlers. Does NOT guard against miscasting. Use with care.
    template<class T>
    T* nodeAs()
    {
        return static_cast<T*>(second);
    }
    
    template<class T>
    bool is() const
    {
        return 0 != dynamic_cast<T*>(second);
    }

    GeoTagHandler::QualifiedName qualifiedName() const { return first; }
    GeoNode* associatedNode() const { return second; }

private:
    friend class GeoParser;
    void assignNode( GeoNode* node ) { second = node; }
};

}

#endif
