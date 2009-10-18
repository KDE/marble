//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin      <ps_ml@gmx.de>
//

//
// This class is the geodata layer plugin.
//

#ifndef MARBLE_GEORENDERER_PLUGIN_H
#define MARBLE_GEORENDERER_PLUGIN_H

#include <QtCore/QObject>

#include "RenderPlugin.h"


namespace Marble
{

class GeoRendererView;

/**
 * @short The class that specifies the Marble layer interface of the plugin.
 *
 * GeoRendererPlugin is the beginning of a Render plugin for vectorized data.
 * This includes data which is generated at runtime as well as data that
 */

class GeoRendererPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(GeoRendererPlugin)

    GeoRendererView* m_view;
 public:
    GeoRendererPlugin();
    ~GeoRendererPlugin();

    QStringList backendTypes() const;

    QString renderPolicy() const;

    QStringList renderPosition() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString description() const;

    QIcon icon () const;


    void initialize ();

    bool isInitialized () const;

    bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer = 0 );
 private:
    bool m_isInitialized;
};

}

#endif
