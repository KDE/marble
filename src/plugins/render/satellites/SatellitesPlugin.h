// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
// SPDX-FileCopyrightText: 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_SATELLITESPLUGIN_H
#define MARBLE_SATELLITESPLUGIN_H

#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"
#include "SatellitesConfigDialog.h"
#include "SatellitesModel.h"

#include <QHash>

namespace Marble
{

class SatellitesConfigModel;

/**
 * @brief This plugin displays satellites and their orbits.
 *
 */
class SatellitesPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.SatellitesPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(SatellitesPlugin)

public:
    explicit SatellitesPlugin(const MarbleModel *marbleModel = nullptr);
    ~SatellitesPlugin() override;

    QStringList backendTypes() const override;
    QString renderPolicy() const override;
    QStringList renderPosition() const override;
    QString name() const override;
    QString nameId() const override;
    QString guiString() const override;
    QString version() const override;
    QString description() const override;
    QString copyrightYears() const override;
    QList<PluginAuthor> pluginAuthors() const override;
    QString aboutDataText() const override;
    QIcon icon() const override;
    RenderType renderType() const override;
    void initialize() override;
    bool isInitialized() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer) override;

    bool eventFilter(QObject *object, QEvent *event) override;

    QHash<QString, QVariant> settings() const override;
    void setSettings(const QHash<QString, QVariant> &settings) override;

    SatellitesConfigDialog *configDialog() override;

private Q_SLOTS:
    void activate();
    void enableModel(bool enabled);
    void visibleModel(bool visible);
    void readSettings();
    void writeSettings();
    void updateSettings();
    void updateDataSourceConfig(const QString &source);
    void dataSourceParsed(const QString &source);
    void userDataSourceAdded(const QString &source);

    void showOrbit(bool show);
    void trackPlacemark();

protected:
    void activateDataSource(const QString &source);
    void addBuiltInDataSources();

private:
    SatellitesModel *m_satModel = nullptr;
    SatellitesConfigModel *m_configModel = nullptr;

    bool m_isInitialized;
    QHash<QString, QVariant> m_settings;
    QStringList m_newDataSources;

    SatellitesConfigDialog *m_configDialog = nullptr;

    QAction *m_showOrbitAction;
    QAction *m_trackPlacemarkAction;
    QList<TrackerPluginItem *> m_trackerList;
};

} // namespace Marble

#endif // MARBLE_SATELLITESPLUGIN_H
