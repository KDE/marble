//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef WEATHERPLUGIN_H
#define WEATHERPLUGIN_H

#include "AbstractDataPlugin.h"
#include "DialogConfigurationInterface.h"

// Qt
#include <QHash>
#include <QIcon>

namespace Ui
{
    class WeatherConfigWidget;
}

namespace Marble
{

class WeatherPlugin : public AbstractDataPlugin, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.WeatherPlugin")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( WeatherPlugin )
    
 public:
    WeatherPlugin();

    explicit WeatherPlugin( const MarbleModel *marbleModel );

    ~WeatherPlugin() override;
    
    void initialize() override;

    QString name() const override;
    
    QString guiString() const override;

    QString nameId() const override;
    
    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString aboutDataText() const override;

    QIcon icon() const override;

    QDialog *configDialog() override;

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString,QVariant> &settings ) override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

 private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateItemSettings();
    void favoriteItemsChanged( const QStringList& favoriteItems ) override;
    
 Q_SIGNALS:
    void changedSettings();

 private:
    void updateSettings();

    quint32 m_updateInterval;
    const QIcon m_icon;
    QDialog * m_configDialog;
    Ui::WeatherConfigWidget * ui_configWidget;

    QHash<QString,QVariant> m_settings;
};

}

#endif // WEATHERPLUGIN_H
