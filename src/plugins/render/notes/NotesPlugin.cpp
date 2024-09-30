// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesPlugin.h"
#include "MarbleDirs.h"
#include "NotesModel.h"

#include <QIcon>

using namespace Marble;

NotesPlugin::NotesPlugin()
    : AbstractDataPlugin(nullptr)
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

QList<PluginAuthor> NotesPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Spencer Brown"), QStringLiteral("spencerbrown991@gmail.com"));
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
