// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef WIKIPEDIAPLUGIN_H
#define WIKIPEDIAPLUGIN_H

#include "AbstractDataPlugin.h"
#include "DialogConfigurationInterface.h"

#include <QHash>
#include <QIcon>

namespace Ui
{
class WikipediaConfigWidget;
}

namespace Marble
{

class WikipediaPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.WikipediaPlugin")
    Q_INTERFACES(Marble::RenderPluginInterface)
    Q_INTERFACES(Marble::DialogConfigurationInterface)
    MARBLE_PLUGIN(WikipediaPlugin)

public:
    WikipediaPlugin();
    explicit WikipediaPlugin(const MarbleModel *marbleModel);
    ~WikipediaPlugin() override;

    void initialize() override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString copyrightYears() const override;

    QString description() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QString aboutDataText() const override;

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
    const QIcon m_icon;
    Ui::WikipediaConfigWidget *ui_configWidget = nullptr;
    QDialog *m_configDialog = nullptr;
    bool m_showThumbnails;
};

}

#endif // WIKIPEDIAPLUGIN_H
