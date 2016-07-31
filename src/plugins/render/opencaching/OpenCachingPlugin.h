//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGPLUGIN_H
#define OPENCACHINGPLUGIN_H

#include "AbstractDataPlugin.h"

#include <QHash>

namespace Ui
{
    class OpenCachingConfigWidget;
}

namespace Marble
{

/**
 * Plugin to display geocaches from opencaching.de on the map.
 */
class OpenCachingPlugin : public AbstractDataPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OpenCachingPlugin")

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( OpenCachingPlugin )

public:
    OpenCachingPlugin();

    virtual void initialize();

    virtual bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString description() const;

    QIcon icon() const;

    QDialog *configDialog();

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( const QHash<QString,QVariant> &settings );

public Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateSettings();

private:
    bool m_isInitialized;
    Ui::OpenCachingConfigWidget *m_ui;
    QDialog *m_configDialog;
    QHash<QString,QVariant> m_settings;

private Q_SLOTS:
    void validateDateRange();
    void validateDifficultyRange();
};

}

#endif // OPENCACHINGPLUGIN_H
