// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2011 Jens-Michael Hoffmann <jmho@c-xx.com>
//

#ifndef MARBLE_ATMOSPHEREPLUGIN_H
#define MARBLE_ATMOSPHEREPLUGIN_H

#include "RenderPlugin.h"

#include <QPixmap>

namespace Marble
{

class AtmospherePlugin : public RenderPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.AtmospherePlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    MARBLE_PLUGIN(AtmospherePlugin)

public:
    AtmospherePlugin();

    explicit AtmospherePlugin(const MarbleModel *marbleModel);

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    RenderType renderType() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QIcon icon() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    qreal zValue() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;
    void repaintPixmap(const ViewportParams *viewParams);

public Q_SLOTS:
    void updateTheme();

private:
    QPixmap m_renderPixmap;
    QColor m_renderColor;
    int m_renderRadius;
};

}

#endif // MARBLE_ATMOSPHEREPLUGIN_H
