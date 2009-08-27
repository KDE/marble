//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Patrick Spendrin      <ps_ml@gmx.de>
// Copyright 2008 Simon Schmeisser      <mail_to_wrt@gmx.de>
//

//
// This class is the placemark layer plugin.
//

#ifndef MARBLEDATAVIEWPLUGIN_H
#define MARBLEDATAVIEWPLUGIN_H

#include <QtCore/QObject>
#include <QtGui/QBrush>
#include <QtGui/QPen>

#include "RenderPlugin.h"

class QTreeView;




namespace Marble
{

class GeoDataDebugModel;

/**
 * @short The class that specifies the Marble layer interface of the plugin.
 *
 * DataViewPlugin is the beginning of a plugin, that displays the geodata as it is stored internally
 */

class DataViewPlugin : public RenderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN(DataViewPlugin)

    public:

    DataViewPlugin();
    ~DataViewPlugin();

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
    QTreeView *m_dataView;
    GeoDataDebugModel *m_debugModel;
};

}
#endif
