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

#include <QDebug>

#include "KMLFolderTagHandler.h"

#include "KMLElementDictionary.h"
#include "GeoDataFolder.h"
#include "GeoParser.h"

using namespace GeoDataElementDictionary;

KML_DEFINE_TAG_HANDLER(Folder)

KMLFolderTagHandler::KMLFolderTagHandler()
    : GeoTagHandler()
{
}

KMLFolderTagHandler::~KMLFolderTagHandler()
{
}

void KMLFolderTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(kmlTag_Folder));

    GeoDataFolder folder;
//    parser.document().addFolder(folder);

    // TODO:
    // A <Folder> contains a <Name>. All you need to do now is to create a new GeoDataFolderNameHandler.
    // Once GeoDataFolderNameHandler::parse() is fired you need to check the last parsed element's tag.
    // If it's ie. a <Folder> you know that parser.document().folders().last() is the one you're supposed
    // to se the name on. Easy, eh?

    qDebug() << "Parsed <Folder> start!";    
}
