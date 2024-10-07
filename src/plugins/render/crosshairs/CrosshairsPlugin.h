// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2010 Cezar Mocan <mocancezar@gmail.com>
//

//
// This class is a crosshairs plugin.
//

#ifndef MARBLE_CROSSHAIRSPLUGIN_H
#define MARBLE_CROSSHAIRSPLUGIN_H

#include <QPixmap>

#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"

class QSvgRenderer;

namespace Ui
{
class CrosshairsConfigWidget;
}

namespace Marble
{

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class CrosshairsPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CrosshairsPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(CrosshairsPlugin)

public:
    CrosshairsPlugin();

    explicit CrosshairsPlugin(const MarbleModel *marbleModel);

    ~CrosshairsPlugin() override;

    QStringList backendTypes() const override;

    QString renderPolicy() const override;

    QStringList renderPosition() const override;

    RenderType renderType() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    void initialize() override;

    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer = nullptr) override;

    QDialog *configDialog() override;

    QHash<QString, QVariant> settings() const override;

    void setSettings(const QHash<QString, QVariant> &settings) override;

private Q_SLOTS:
    void readSettings();

    void writeSettings();

private:
    Q_DISABLE_COPY(CrosshairsPlugin)

    bool m_isInitialized;

    QSvgRenderer *m_svgobj = nullptr;
    QPixmap m_crosshairs;
    int m_themeIndex;

    QString m_theme;

    QDialog *m_configDialog = nullptr;
    Ui::CrosshairsConfigWidget *m_uiConfigWidget = nullptr;
};

}

#endif // MARBLE_CROSSHAIRSPLUGIN_H
