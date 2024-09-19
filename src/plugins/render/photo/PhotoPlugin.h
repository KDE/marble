// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef PHOTOPLUGIN_H
#define PHOTOPLUGIN_H

#include "AbstractDataPlugin.h"
#include "DialogConfigurationInterface.h"

#include <QHash>

namespace Ui
{
class PhotoConfigWidget;
}

namespace Marble
{

class PhotoPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.PhotoPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(PhotoPlugin)

public:
    PhotoPlugin();

    explicit PhotoPlugin(const MarbleModel *marbleModel);

    ~PhotoPlugin() override;

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    /**
     * @return: The settings of the item.
     */
    QHash<QString, QVariant> settings() const override;

    /**
     * Set the settings of the item.
     */
    void setSettings(const QHash<QString, QVariant> &settings) override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private Q_SLOTS:
    void readSettings();
    void writeSettings();

    void updateSettings();
    void checkNumberOfItems(quint32 number);

private:
    Ui::PhotoConfigWidget *ui_configWidget;
    QDialog *m_configDialog;

    QStringList m_checkStateList;
};

}

#endif // PHOTOPLUGIN_H
