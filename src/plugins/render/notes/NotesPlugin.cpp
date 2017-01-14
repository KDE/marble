//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesPlugin.h"
#include "NotesModel.h"
#include "MarbleDirs.h"

#include <QIcon>

using namespace Marble;

NotesPlugin::NotesPlugin()
    : AbstractDataPlugin(0)
{
}

NotesPlugin::NotesPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
{
    setEnabled(true);
    setVisible(false);
}

void NotesPlugin::initialize()
{
    setModel(new NotesModel(marbleModel(), this));
    setNumberOfItems(20);
}

QString NotesPlugin::name() const
{
    return tr("OSM Mapper Notes");
}

QString NotesPlugin::guiString() const
{
    return name();
}

QString NotesPlugin::nameId() const
{
    return QStringLiteral("notes");
}

QString NotesPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString NotesPlugin::copyrightYears() const
{
    return QStringLiteral("2017");
}

QVector<PluginAuthor> NotesPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
           << PluginAuthor(QStringLiteral("Spencer Brown"), QStringLiteral("spencerbrown991@gmail.com"));
}

QString NotesPlugin::description() const
{
    return tr("Display OpenStreetMap Mapper Notes.");
}

QIcon NotesPlugin::icon() const
{
    return QIcon(MarbleDirs::path("bitmaps/notes_open.png"));
}

#include "moc_NotesPlugin.cpp"
