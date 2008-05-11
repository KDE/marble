//
// This file is part of the Marble Desktop Globe.
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

#include "MarbleLayerInterface.h"

/**
 * \brief This class displays a layer of satellites (which satellites TBD).
 *
 */
class SatellitesPlugin : public QObject, public MarbleLayerInterface
{
    Q_OBJECT
    Q_INTERFACES( MarbleLayerInterface )

 public:
    QStringList backendTypes() const;
    QString renderPolicy() const;
    QString renderPosition() const;
    QString name() const;
    QString nameId() const;
    QString description() const;
    QIcon icon () const;

    void initialize ();
    bool isInitialized () const;
    bool render( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer = 0 );

//    QTimer* m_timer; /**< Timer to set the update interval */
};

#endif
