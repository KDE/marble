//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESPLUGIN_H
#define MARBLE_SATELLITESPLUGIN_H

#include "RenderPlugin.h"
#include "SatellitesModel.h"

#include "sgp4/sgp4unit.h"

#include <QtCore/QObject>

class QCheckBox;

namespace Ui
{
    class SatellitesConfigDialog;
}

namespace Marble
{

class PluginAboutDialog;

/**
 * @brief This plugin displays satellites and their orbits.
 *
 */
class SatellitesPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( SatellitesPlugin )

public:
    SatellitesPlugin();
    virtual ~SatellitesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString nameId() const;
    QString guiString() const;
    QString description() const;
    QIcon icon() const;
    void initialize();
    bool isInitialized() const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer );

    QHash<QString, QVariant> settings() const;
    void setSettings( QHash<QString, QVariant> settings );

    QDialog *aboutDialog();
    QDialog *configDialog();

private Q_SLOTS:
    void readSettings();
    void writeSettings();
    void updateSettings();

private:
    SatellitesModel *m_model;

    bool m_isInitialized;
    QHash<QString, QVariant> m_settings;
    QHash<QString, QCheckBox *> m_boxHash;

    PluginAboutDialog *m_aboutDialog;
    QDialog *m_configDialog;
    Ui::SatellitesConfigDialog *ui_configWidget;

    void populateBoxHash();
};

}

#endif // MARBLE_SATELLITESPLUGIN_H
