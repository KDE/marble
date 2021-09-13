// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Spencer Brown <spencerbrown991@gmail.com>
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
