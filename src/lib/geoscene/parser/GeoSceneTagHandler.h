/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef GeoSceneTagHandler_h
#define GeoSceneTagHandler_h

#include <QHash>

class GeoSceneParser;

class GeoSceneTagHandler {
public:
    // API to be implemented by child handlers.
    virtual void parse(GeoSceneParser&) const = 0;

protected: // This base class is not directly constructable nor is it copyable.
    GeoSceneTagHandler();
    virtual ~GeoSceneTagHandler();

private:
    GeoSceneTagHandler(const GeoSceneTagHandler&);
    GeoSceneTagHandler& operator=(const GeoSceneTagHandler&);

private: // Only our registrar is allowed to register tag handlers.
    friend class GeoSceneTagHandlerRegistrar;
    static void registerHandler(const QString& tagName, const GeoSceneTagHandler*);

private: // Only our parser is allowed to access tag handlers.
    friend class GeoSceneParser;
    static const GeoSceneTagHandler* recognizes(const QString& tagName);

private:
    typedef QHash<QString, const GeoSceneTagHandler*> TagHash;

    static TagHash* tagHandlerHash();
    static TagHash* s_tagHandlerHash;
};

// Helper structure
struct GeoSceneTagHandlerRegistrar {
public:
    GeoSceneTagHandlerRegistrar(const QString& tagName, const GeoSceneTagHandler* handler)
    {
        GeoSceneTagHandler::registerHandler(tagName, handler);
    }
};

// Macros to ease registering new handlers
#define GEODATA_DEFINE_TAG_HANDLER(Module, UpperCaseModule, Name) \
    static GeoSceneTagHandlerRegistrar s_myTagHandler(Module##Tag_##Name, new UpperCaseModule##Name##TagHandler());

#endif // GeoSceneTagHandler_h
