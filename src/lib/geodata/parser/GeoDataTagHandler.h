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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef GeoDataTagHandler_h
#define GeoDataTagHandler_h

#include <QHash>

class GeoDataParser;

class GeoDataTagHandler {
public:
    // API to be implemented by child handlers.
    virtual void parse(GeoDataParser&) const = 0;

protected: // This base class is not directly constructable nor is it copyable.
    GeoDataTagHandler();
    virtual ~GeoDataTagHandler();

private:
    GeoDataTagHandler(const GeoDataTagHandler&);
    GeoDataTagHandler& operator=(const GeoDataTagHandler&);

private: // Only our registrar is allowed to register tag handlers.
    friend class GeoDataTagHandlerRegistrar;
    static void registerHandler(const QString& tagName, const GeoDataTagHandler*);

private: // Only our parser is allowed to access tag handlers.
    friend class GeoDataParser;
    static const GeoDataTagHandler* recognizes(const QString& tagName);

private:
    typedef QHash<QString, const GeoDataTagHandler*> TagHash;

    static TagHash* tagHandlerHash();
    static TagHash* s_tagHandlerHash;
};

// Helper structure
struct GeoDataTagHandlerRegistrar {
public:
    GeoDataTagHandlerRegistrar(const QString& tagName, const GeoDataTagHandler* handler)
    {
        GeoDataTagHandler::registerHandler(tagName, handler);
    }
};

// Macros to ease registering new handlers
#define GEODATA_DEFINE_TAG_HANDLER(Module, UpperCaseModule, Name) \
    static GeoDataTagHandlerRegistrar s_myTagHandler(Module##Tag_##Name, new UpperCaseModule##Name##TagHandler());

#endif // GeoDataTagHandler_h
