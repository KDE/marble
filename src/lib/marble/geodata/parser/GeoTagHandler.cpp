/*
    SPDX-FileCopyrightText: 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "GeoTagHandler.h"

// Marble
#include "MarbleDebug.h"

namespace Marble
{

// Set to a value greater than 0, to dump tag handlers as they get registered
#define DUMP_TAG_HANDLER_REGISTRATION 0

GeoTagHandler::TagHash *GeoTagHandler::s_tagHandlerHash = nullptr;

GeoTagHandler::GeoTagHandler() = default;

GeoTagHandler::~GeoTagHandler() = default;

GeoTagHandler::TagHash *GeoTagHandler::tagHandlerHash()
{
    if (!s_tagHandlerHash)
        s_tagHandlerHash = new TagHash();

    Q_ASSERT(s_tagHandlerHash);
    return s_tagHandlerHash;
}

void GeoTagHandler::registerHandler(const GeoParser::QualifiedName &qName, const GeoTagHandler *handler)
{
    TagHash *hash = tagHandlerHash();

    Q_ASSERT(!hash->contains(qName));
    hash->insert(qName, handler);
    Q_ASSERT(hash->contains(qName));

#if DUMP_TAG_HANDLER_REGISTRATION > 0
    mDebug() << "[GeoTagHandler] -> Recognizing" << qName.first << "tag with namespace" << qName.second;
#endif
}

void GeoTagHandler::unregisterHandler(const GeoParser::QualifiedName &qName)
{
    TagHash *hash = tagHandlerHash();

    Q_ASSERT(hash->contains(qName));
    delete hash->value(qName);
    hash->remove(qName);
    Q_ASSERT(!hash->contains(qName));
}

const GeoTagHandler *GeoTagHandler::recognizes(const GeoParser::QualifiedName &qName)
{
    TagHash *hash = tagHandlerHash();

    if (!hash->contains(qName))
        return nullptr;

    return (*hash)[qName];
}

}
