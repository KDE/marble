//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_SATELLITESPLUGIN_H
#define MARBLE_SATELLITESPLUGIN_H

#include "RenderPlugin.h"
#include "DialogConfigurationInterface.h"
#include "EarthSatellitesModel.h"
#include "OrbiterSatellitesModel.h"

#include "sgp4/sgp4unit.h"

#include <QtCore/QObject>

class QCheckBox;

namespace Ui
{
    class SatellitesConfigDialog;
}

namespace Marble
{

class SatellitesConfigModel;

/**
 * @brief This plugin displays satellites and their orbits.
 *
 */
class SatellitesPlugin : public RenderPlugin,
                         public DialogConfigurationInterface
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( SatellitesPlugin )

public:
    SatellitesPlugin();
    explicit SatellitesPlugin( const MarbleModel *marbleModel );
    virtual ~SatellitesPlugin();
    
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QStringList renderPosition() const;
    QString name() const;
    QString nameId() const;
    QString guiString() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QList<PluginAuthor> pluginAuthors() const;
    QString aboutDataText() const;
    QIcon icon() const;
    RenderType renderType() const;
    void initialize();
    bool isInitialized() const;

    bool render( GeoPainter *painter,
                 ViewportParams *viewport,
                 const QString &renderPos,
                 GeoSceneLayer *layer );

    QHash<QString, QVariant> settings() const;
    void setSettings( const QHash<QString, QVariant> &settings );

    QDialog *configDialog();

private Q_SLOTS:
    void enableModel( bool enabled );
    void visibleModel( bool visible );
    void readSettings();
    void writeSettings();
    void updateSettings();

private:
    void setupConfigModel();

    EarthSatellitesModel *m_earthSatModel;
    OrbiterSatellitesModel *m_orbiterSatModel;

    bool m_isInitialized;
    QHash<QString, QVariant> m_settings;
    QHash<QString, QCheckBox *> m_boxHash;
    QString m_currentPlanet;

    QDialog *m_configDialog;
    SatellitesConfigModel *m_configModel;
    Ui::SatellitesConfigDialog *ui_configWidget;
};

}

#endif // MARBLE_SATELLITESPLUGIN_H
