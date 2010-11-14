//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

//
// This class is a crosshairs plugin.
//

#ifndef MARBLECROSSHAIRSPLUGIN_H
#define MARBLECROSSHAIRSPLUGIN_H

#include <QtCore/QObject>

#include "RenderPlugin.h"


namespace Marble
{


/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class CrosshairsPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(CrosshairsPlugin)

 public:
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
};

}

#endif // MARBLECROSSHAIRSPLUGIN_H
