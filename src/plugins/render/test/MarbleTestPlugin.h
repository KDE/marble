//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

//
// This class is a test plugin.
//

#ifndef MARBLETESTPLUGIN_H
#define MARBLETESTPLUGIN_H

#include <QtCore/QObject>

#include "MarbleRenderPlugin.h"

namespace Marble
{

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class MarbleTestPlugin : public MarbleRenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::MarbleRenderPluginInterface )
    MARBLE_PLUGIN( MarbleTestPlugin )

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

#endif // MARBLETESTPLUGIN_H
