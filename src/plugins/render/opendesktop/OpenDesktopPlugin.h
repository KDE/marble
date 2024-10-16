// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef OPENDESKTOPPLUGIN_H
#define OPENDESKTOPPLUGIN_H

#include "AbstractDataPlugin.h"
#include "DialogConfigurationInterface.h"

namespace Ui
{
class OpenDesktopConfigWidget;
}

namespace Marble
{

const int defaultItemsOnScreen = 15;

class OpenDesktopPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OpenDesktopPlugin")

    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)

    MARBLE_PLUGIN(OpenDesktopPlugin)

public:
    OpenDesktopPlugin();

    explicit OpenDesktopPlugin(const MarbleModel *marbleModel);

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    QHash<QString, QVariant> settings() const override;

    void setSettings(const QHash<QString, QVariant> &settings) override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private Q_SLOTS:
    void readSettings();

    void writeSettings();

private:
    QDialog *m_configDialog = nullptr;
    Ui::OpenDesktopConfigWidget *m_uiConfigWidget;
};

}

#endif // OPENDESKTOPPLUGIN_H
