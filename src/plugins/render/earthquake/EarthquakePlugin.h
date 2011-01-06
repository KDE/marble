//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#ifndef EARTHQUAKEPLUGIN_H
#define EARTHQUAKEPLUGIN_H

#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "RenderPluginInterface.h"

#include <QtCore/QHash>
#include <QtGui/QIcon>
#include <QtGui/QAbstractButton>

namespace Ui
{
    class EarthquakeConfigWidget;
}

namespace Marble
{

class EarthquakePlugin : public AbstractDataPlugin
{
    Q_OBJECT

    Q_INTERFACES( Marble::RenderPluginInterface )

    MARBLE_PLUGIN( EarthquakePlugin )

public:
    EarthquakePlugin();

    virtual void initialize();

    virtual bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString description() const;

    QIcon icon() const;

    QDialog *configDialog() const;

    /**
     * @return: The settings of the item.
     */
    virtual QHash<QString,QVariant> settings() const;

    /**
     * Set the settings of the item.
     */
    virtual void setSettings( QHash<QString,QVariant> settings );

public slots:
    void readSettings() const;
    void writeSettings();
    void updateSettings();

private:
    bool m_isInitialized;
    mutable Ui::EarthquakeConfigWidget *ui_configWidget;
    mutable QDialog *m_configDialog;
    QHash<QString,QVariant> m_settings;

private slots:
    void validateDateRange();
    void evaluateClickedButton( QAbstractButton *button );
};

}

#endif // EARTHQUAKEPLUGIN_H
