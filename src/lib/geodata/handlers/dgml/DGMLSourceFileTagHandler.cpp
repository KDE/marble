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

#include "DGMLSourceFileTagHandler.h"

#include "DGMLElementDictionary.h"
#include "DGMLAttributeDictionary.h"
#include "GeoParser.h"
#include "GeoSceneVector.h"

using namespace GeoSceneElementDictionary;
using namespace GeoSceneAttributeDictionary;

DGML_DEFINE_TAG_HANDLER(SourceFile)

DGMLSourceFileTagHandler::DGMLSourceFileTagHandler()
    : GeoTagHandler()
{
}

DGMLSourceFileTagHandler::~DGMLSourceFileTagHandler()
{
}

GeoNode* DGMLSourceFileTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_SourceFile));

    QString format = parser.attribute(dgmlAttr_format).trimmed();

    GeoSceneVector *vector = 0;

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Vector)) {
        vector = parentItem.nodeAs<GeoSceneVector>();
        vector->setSourceFile( parser.readElementText().trimmed() );
        vector->setFileFormat(format);
    }

    return 0;
}
