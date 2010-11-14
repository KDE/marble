//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Claudiu Covaci <claudiu.covaci@gmail.com>
//

#ifndef SATELLITESPLUGIN_H
#define SATELLITESPLUGIN_H

#include <QtCore/QObject>

#include "RenderPlugin.h"

namespace Marble
{

/**
 * \brief This class displays a layer of satellites (which satellites TBD).
 *
 */
class SatellitesPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( SatellitesPlugin )

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

//    QTimer* m_timer; /**< Timer to set the update interval */
};

}

#endif
