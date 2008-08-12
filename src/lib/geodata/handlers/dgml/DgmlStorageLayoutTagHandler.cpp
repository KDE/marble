/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

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

#include "DgmlStorageLayoutTagHandler.h"

#include <QtCore/QDebug>

#include "DgmlAttributeDictionary.h"
#include "DgmlElementDictionary.h"
#include "GeoParser.h"
#include "GeoSceneTexture.h"

namespace Marble
{

using namespace GeoSceneAttributeDictionary;
using namespace GeoSceneElementDictionary;

DGML_DEFINE_TAG_HANDLER(StorageLayout)

DgmlStorageLayoutTagHandler::DgmlStorageLayoutTagHandler()
    : GeoTagHandler()
{
}

DgmlStorageLayoutTagHandler::~DgmlStorageLayoutTagHandler()
{
}

GeoNode* DgmlStorageLayoutTagHandler::parse(GeoParser& parser) const
{
    // Check whether the tag is valid
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(dgmlTag_StorageLayout));

    // Attribute levelZeroColumns, default to value of the oldest tile themes
    int levelZeroColumns = 2;
    const QString levelZeroColumnsStr = parser.attribute(dgmlAttr_levelZeroColumns).trimmed();
    if ( !levelZeroColumnsStr.isEmpty() ) {
        levelZeroColumns = levelZeroColumnsStr.toInt();
    }

    // Attribute levelZeroRows, default to value of the oldest tile themes
    int levelZeroRows = 1;
    const QString levelZeroRowsStr = parser.attribute(dgmlAttr_levelZeroRows).trimmed();
    if ( !levelZeroRowsStr.isEmpty() ) {
        levelZeroRows = levelZeroRowsStr.toInt();
    }

    // Attribute mode
    GeoSceneTexture::StorageLayoutMode mode = GeoSceneTexture::Marble;
    const QString modeStr = parser.attribute(dgmlAttr_mode).trimmed();
    if ( modeStr == "OpenStreetMap" )
        mode = GeoSceneTexture::OpenStreetMap;
    else if ( modeStr == "Custom" )
        mode = GeoSceneTexture::Custom;

    // custom Layout?
    QString customLayout;
    if ( mode == GeoSceneTexture::Custom ) {
        customLayout = parser.readElementText().trimmed();
    }

    // Checking for parent item
    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.represents(dgmlTag_Texture)) {
        parentItem.nodeAs<GeoSceneTexture>()->setLevelZeroColumns( levelZeroColumns );
        parentItem.nodeAs<GeoSceneTexture>()->setLevelZeroRows( levelZeroRows );
        qDebug() << "level zero columns x rows:" << levelZeroColumns << 'x' << levelZeroRows;
        parentItem.nodeAs<GeoSceneTexture>()->setStorageLayoutMode( mode );
	if ( mode == GeoSceneTexture::Custom )
            parentItem.nodeAs<GeoSceneTexture>()->setCustomStorageLayout( customLayout );
    }

    return 0;
}

}
