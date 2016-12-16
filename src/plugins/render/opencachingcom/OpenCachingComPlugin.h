//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#ifndef OPENCACHINGCOMPLUGIN_H
#define OPENCACHINGCOMPLUGIN_H

#include "AbstractDataPlugin.h"

namespace Ui
{
//     class OpenCachingConfigWidget;
}

namespace Marble
{

/**
 * Plugin to display geocaches from opencaching.com on the map.
 */
class OpenCachingComPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OpenCachingComPlugin")

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( OpenCachingComPlugin )

public:
    OpenCachingComPlugin();
    explicit OpenCachingComPlugin(const MarbleModel *marbleModel);

    void initialize() override;

    bool isInitialized() const override;

    QStringList backendTypes() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog();

private:
    bool m_isInitialized;

};

}

#endif // OPENCACHINGCOMPLUGIN_H
