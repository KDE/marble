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

    ~WeatherPlugin();
    
    void initialize();

    QString name() const;
    
    QString guiString() const;

    QString nameId() const;
    
    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString aboutDataText() const;

    QIcon icon() const;

    QDialog *configDialog();

    QHash<QString,QVariant> settings() const;

    void setSettings( const QHash<QString,QVariant> &settings );

protected:
    bool eventFilter(QObject *object, QEvent *event);

 private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateItemSettings();
    void favoriteItemsChanged( const QStringList& favoriteItems );
    
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
