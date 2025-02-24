// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSPLUGIN_H
#define APRSPLUGIN_H

#include <QDialog>

#include "AprsGatherer.h"
#include "AprsObject.h"
#include "DialogConfigurationInterface.h"
#include "GeoDataLatLonAltBox.h"
#include "RenderPlugin.h"

#include "ui_AprsConfigWidget.h"

class QMutex;

namespace Ui
{
class AprsConfigWidget;
}

namespace Marble
{

/**
 * \brief This class displays a layer of aprs (which aprs TBD).
 *
 */
class AprsPlugin : public RenderPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.AprsPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(AprsPlugin)

public:
    explicit AprsPlugin(const MarbleModel *marbleModel = nullptr);
    ~AprsPlugin() override;
    QStringList backendTypes() const override;
    QString renderPolicy() const override;
    QStringList renderPosition() const override;
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
    QAction *action() const;

    QHash<QString, QVariant> settings() const override;
    void setSettings(const QHash<QString, QVariant> &settings) override;

    void stopGatherers();
    void restartGatherers();

private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateVisibility(bool visible);
    RenderType renderType() const override;

private:
    QMutex *m_mutex;
    QMap<QString, AprsObject *> m_objects;
    bool m_initialized;
    GeoDataLatLonAltBox m_lastBox;
    AprsGatherer *m_tcpipGatherer, *m_ttyGatherer, *m_fileGatherer;
    QString m_filter;
    QAction *m_action;

    bool m_useInternet;
    bool m_useTty;
    bool m_useFile;
    QString m_aprsHost;
    int m_aprsPort;
    QString m_tncTty;
    QString m_aprsFile;
    bool m_dumpTcpIp;
    bool m_dumpTty;
    bool m_dumpFile;
    int m_fadeTime;
    int m_hideTime;

    QDialog *m_configDialog;
    Ui::AprsConfigWidget *ui_configWidget;
};

}

#endif
