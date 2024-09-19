// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
//

//
// This class is a test plugin.
//

#ifndef MARBLETESTPLUGIN_H
#define MARBLETESTPLUGIN_H

#include "RenderPlugin.h"

namespace Marble
{

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class TestPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.TestPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    MARBLE_PLUGIN(TestPlugin)

public:
    TestPlugin();
    explicit TestPlugin(const MarbleModel *marbleModel);

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const override;

    QString description() const;

    QIcon icon() const;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    void initialize();

    bool isInitialized() const;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = 0);
};

}

#endif // MARBLETESTPLUGIN_H
