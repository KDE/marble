/*
    Copyright (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

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


// Own
#include "GeoTagHandler.h"

// Marble
#include "MarbleDebug.h"


namespace Marble
{

// Set to a value greater than 0, to dump tag handlers as they get registered
#define DUMP_TAG_HANDLER_REGISTRATION 0

GeoTagHandler::TagHash* GeoTagHandler::s_tagHandlerHash = 0;

GeoTagHandler::GeoTagHandler()
{
}

GeoTagHandler::~GeoTagHandler()
{
}

GeoTagHandler::TagHash* GeoTagHandler::tagHandlerHash()
{
    if (!s_tagHandlerHash)
        s_tagHandlerHash = new TagHash();

    Q_ASSERT(s_tagHandlerHash);
    return s_tagHandlerHash;
}

void GeoTagHandler::registerHandler(const GeoParser::QualifiedName& qName, const GeoTagHandler* handler)
{
    TagHash* hash = tagHandlerHash();

    Q_ASSERT(!hash->contains(qName));
    hash->insert(qName, handler);
    Q_ASSERT(hash->contains(qName));

#if DUMP_TAG_HANDLER_REGISTRATION > 0
    mDebug() << "[GeoTagHandler] -> Recognizing" << qName.first << "tag with namespace" << qName.second;
#endif
}

void GeoTagHandler::unregisterHandler(const GeoParser::QualifiedName& qName)
{
    TagHash* hash = tagHandlerHash();

    Q_ASSERT(hash->contains(qName));
    hash->remove(qName);
    Q_ASSERT(!hash->contains(qName));
}

const GeoTagHandler* GeoTagHandler::recognizes(const GeoParser::QualifiedName& qName)
{
    TagHash* hash = tagHandlerHash();

    if (!hash->contains(qName))
        return 0;

    return (*hash)[qName];
}

}
