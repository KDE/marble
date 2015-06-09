//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef INHIBITSCREENSAVERPLUGIN_H
#define INHIBITSCREENSAVERPLUGIN_H

#include "RenderPlugin.h"

namespace Marble
{
class PositionProviderPlugin;
class InhibitScreensaverPluginPrivate;

/**
  * A plugin that inhibits the screensaver as long as a position
  * provider plugin is active
  */
class InhibitScreensaverPlugin : public RenderPlugin
{

Q_OBJECT
    Q_PLUGIN_METADATA( IID "org.kde.edu.marble.InhibitScreensaverPlugin" )
Q_INTERFACES( Marble::RenderPluginInterface )

MARBLE_PLUGIN( InhibitScreensaverPlugin )

public:
    InhibitScreensaverPlugin();

    explicit InhibitScreensaverPlugin( const MarbleModel *marbleModel );

    ~InhibitScreensaverPlugin();

    QStringList backendTypes() const;

    void initialize();

    bool isInitialized() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QIcon icon() const;

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    virtual QString renderPolicy() const;

private Q_SLOTS:
     void updateScreenSaverState( PositionProviderPlugin *activePlugin );

     void inhibitScreenSaver();

private:
     InhibitScreensaverPluginPrivate* const d;
};

}

#endif // INHIBITSCREENSAVERPLUGIN_H
