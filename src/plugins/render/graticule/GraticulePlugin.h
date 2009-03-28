//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Torsten Rahn <tackat@kde.org>"
//

//
// This class is a graticule plugin.
//

#ifndef MARBLEGRATICULEPLUGIN_H
#define MARBLEGRATICULEPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "RenderPlugin.h"


namespace Marble
{


/**
 * @short A plugin that creates a coordinate grid on top of the map.
 *
 */

class GraticulePlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(GraticulePlugin)

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

 private:
    void renderLatitudeCircle( GeoPainter *painter, qreal latitude );

};

}

#endif // MARBLEGRATICULEPLUGIN_H
