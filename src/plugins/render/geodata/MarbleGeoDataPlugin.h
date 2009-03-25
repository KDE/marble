//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin      <ps_ml@gmx.de>"
//

//
// This class is the geodata layer plugin.
//

#ifndef MARBLEGEODATAPLUGIN_H
#define MARBLEGEODATAPLUGIN_H

#include <QtCore/QObject>

#include "MarbleRenderPlugin.h"


namespace Marble
{

class GeoDataGeometry;
class GeoDataFeature;
class GeoDataDocument;
class MarbleGeoDataView;

/**
 * @short The class that specifies the Marble layer interface of the plugin.
 *
 * MarbleGeoDataPlugin is the beginning of a Render plugin for vectorized data.
 * This includes data which is generated at runtime as well as data that
 */

class MarbleGeoDataPlugin : public MarbleRenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::MarbleRenderPluginInterface )
    MARBLE_PLUGIN(MarbleGeoDataPlugin)

    MarbleGeoDataView* m_view;
 public:
    ~MarbleGeoDataPlugin();

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

#endif
