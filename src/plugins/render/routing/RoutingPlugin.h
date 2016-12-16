//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Siddharth Srivastava <akssps011@gmail.com>
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGPLUGIN_H
#define MARBLE_ROUTINGPLUGIN_H

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"

namespace Marble
{
class RoutingPluginPrivate;
class PositionProviderPlugin;

class RoutingPlugin : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.RoutingPlugin")

    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )

    MARBLE_PLUGIN( RoutingPlugin )

public:
    RoutingPlugin();

    explicit RoutingPlugin( const MarbleModel *marbleModel );

    ~RoutingPlugin() override;

    QStringList backendTypes() const override;

    void initialize() override;

    bool isInitialized() const override;

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon() const override;

    bool eventFilter( QObject *object, QEvent *event ) override;

    QHash<QString,QVariant> settings() const override;

    void setSettings( const QHash<QString,QVariant> &settings ) override;

    QDialog *configDialog() override;

private Q_SLOTS:
    /** Write settings */
    void writeSettings();

private:    
    /** Disable zoom buttons if needed */
    Q_PRIVATE_SLOT( d, void updateZoomButtons() )

    /** Disable zoom buttons if needed */
    Q_PRIVATE_SLOT( d, void updateZoomButtons( int ) )

    /** Switch source/destination of the route and recalculate it */
    Q_PRIVATE_SLOT( d, void reverseRoute() )

    /** Toggles guidance mode */
    Q_PRIVATE_SLOT( d, void toggleGuidanceMode( bool enabled ) )

    /** sets time and distance remaining to reach the destination */
    Q_PRIVATE_SLOT( d, void updateDestinationInformation() )

    /** Update the checked state of the position tracking button */
    Q_PRIVATE_SLOT( d, void updateGpsButton( PositionProviderPlugin *activePlugin ) )

    /** Activate or deactivate position tracking */
    Q_PRIVATE_SLOT( d, void togglePositionTracking( bool enabled ) )

    Q_PRIVATE_SLOT( d, void updateGuidanceModeButton() )

    /** Read settings */
    Q_PRIVATE_SLOT( d, void readSettings() )

    friend class RoutingPluginPrivate;
    RoutingPluginPrivate* const d;
};

}

#endif // MARBLE_ROUTINGPLUGIN_H

