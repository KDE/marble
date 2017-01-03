//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
//

#ifndef NOTESPLUGIN_H
#define NOTESPLUGIN_H

#include "AbstractDataPlugin.h"

namespace Marble
{

class NotesPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.NotesPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    MARBLE_PLUGIN(NotesPlugin)

public:
    NotesPlugin();

    explicit NotesPlugin(const MarbleModel *marbleModel);

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;
};

}

#endif // NOTESPLUGIN_H
