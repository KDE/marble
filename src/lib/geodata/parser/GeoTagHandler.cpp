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

#include <QDebug>

#include "GeoTagHandler.h"

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

void GeoTagHandler::registerHandler(const QString& tagName, const GeoTagHandler* handler)
{
    TagHash* hash = tagHandlerHash();

    Q_ASSERT(!hash->contains(tagName));
    hash->insert(tagName, handler);

    qDebug() << "[GeoTagHandler] -> Recognizing" << tagName << "tag!";
}

const GeoTagHandler* GeoTagHandler::recognizes(const QString& tagName)
{
    TagHash* hash = tagHandlerHash();

    if (!hash->contains(tagName))
        return 0;

    return (*hash)[tagName];
}
