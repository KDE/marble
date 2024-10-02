/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOTAGHANDLER_H
#define MARBLE_GEOTAGHANDLER_H

#include "GeoParser.h"
#include "marble_export.h"
#include <QHash>

namespace Marble
{

class GeoNode;

/**
 * @brief A base class for XML tag handlers
 * This is a base class that is used in the GeoParser architecture. To implement
 * a new GeoData format you will need to subclass GeoTagHandler and reimplement
 * the @see parse(GeoParser&) method. You also need to register the newly
 * implemented GeoTagHandler by declaring an instance of the helper structure
 * @see GeoTagHandlerRegistrar with a corresponding @see QualifiedName.
 */
class MARBLE_EXPORT GeoTagHandler
{
public:
    // API to be implemented by child handlers.
    virtual GeoNode *parse(GeoParser &) const = 0;

protected: // This base class is not directly constructable nor is it copyable.
    GeoTagHandler();
    virtual ~GeoTagHandler();

private:
    GeoTagHandler(const GeoTagHandler &) = delete;
    GeoTagHandler &operator=(const GeoTagHandler &) = delete;

private: // Only our registrar is allowed to register tag handlers.
    friend struct GeoTagHandlerRegistrar;
    static void registerHandler(const GeoParser::QualifiedName &, const GeoTagHandler *);
    static void unregisterHandler(const GeoParser::QualifiedName &);

private: // Only our parser is allowed to access tag handlers.
    friend class GeoParser;
    static const GeoTagHandler *recognizes(const GeoParser::QualifiedName &);

private:
    using TagHash = QHash<GeoParser::QualifiedName, const GeoTagHandler *>;

    static TagHash *tagHandlerHash();
    static TagHash *s_tagHandlerHash;
};

// Helper structure
struct GeoTagHandlerRegistrar {
public:
    GeoTagHandlerRegistrar(const GeoParser::QualifiedName &name, const GeoTagHandler *handler)
        : m_name(name)
    {
        GeoTagHandler::registerHandler(name, handler);
    }

    ~GeoTagHandlerRegistrar()
    {
        GeoTagHandler::unregisterHandler(m_name);
    }

private:
    GeoParser::QualifiedName m_name;
};

// Macros to ease registering new handlers
#define GEODATA_DEFINE_TAG_HANDLER(Module, UpperCaseModule, Name, NameSpace)                                                                                   \
    static GeoTagHandlerRegistrar s_handler##Name##NameSpace(GeoParser::QualifiedName(QLatin1String(Module##Tag_##Name), QLatin1String(NameSpace)),            \
                                                             new UpperCaseModule##Name##TagHandler());

}

#endif
