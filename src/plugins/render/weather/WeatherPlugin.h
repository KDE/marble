//
// This file is part of the Marble Desktop Globe.
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
#include "ui_WeatherConfigWidget.h"

// Qt
#include <QtCore/QHash>

class QIcon;

namespace Marble
{

class PluginAboutDialog;

class WeatherPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( WeatherPlugin )
    
 public:
    WeatherPlugin();
    
    ~WeatherPlugin();
    
    void initialize();
    
    QString name() const;
    
    QString guiString() const;
    
    QString description() const;
    
    QIcon icon() const;

    QDialog *aboutDialog() const;

    QDialog *configDialog() const;

    QHash<QString,QVariant> settings() const;

    void setSettings( QHash<QString,QVariant> settings );

 private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateItemSettings();
    
 Q_SIGNALS:
    void changedSettings();

 private:
    QIcon m_icon;
    PluginAboutDialog *m_aboutDialog;
    QDialog *m_configDialog;
    Ui::WeatherConfigWidget ui_configWidget;
    QHash<QString,QVariant> m_settings;
};

}

#endif // WEATHERPLUGIN_H
