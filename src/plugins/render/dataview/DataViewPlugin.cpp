//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2008 Simon Schmeisser <mail_to_wrt@gmx.de>
//

#include "DataViewPlugin.h"

#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtGui/QTreeView>

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "GeoPainter.h"

#include "GeoDataCoordinates.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFeature.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataParser.h"

#include "GeoDataDebugModel.h"
//#include "modeltest.h"

namespace Marble
{

DataViewPlugin::DataViewPlugin()
: m_dataView(NULL), m_debugModel(NULL)
{
}

DataViewPlugin::~DataViewPlugin()
{
    delete m_dataView;
    delete m_debugModel;
}

QStringList DataViewPlugin::backendTypes() const
{
    return QStringList( "debug" );
}

QString DataViewPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList DataViewPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString DataViewPlugin::name() const
{
    return tr( "DataView Plugin" );
}

QString DataViewPlugin::guiString() const
{
    return tr( "&DataView Plugin" );
}

QString DataViewPlugin::nameId() const
{
    return QString( "DataView-plugin" );
}

QString DataViewPlugin::description() const
{
    return tr( "A plugin that displays all the loaded data in a separate window." );
}

QIcon DataViewPlugin::icon () const
{
    return QIcon();
}


void DataViewPlugin::initialize ()
{
    m_dataView = new QTreeView;
    m_debugModel = new GeoDataDebugModel( dataFacade()->geoDataModel()->geoDataRoot() );
    
    m_dataView->setModel( m_debugModel );
    m_dataView->show();
    
    connect(dataFacade()->geoDataModel(), SIGNAL( dataChanged() ), m_debugModel, SLOT(update()));
   // new ModelTest( m_debugModel );
}

bool DataViewPlugin::isInitialized () const
{
    return true;
}

bool DataViewPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED(painter);
    Q_UNUSED(viewport);
    Q_UNUSED(renderPos);
    Q_UNUSED(layer);
    return true;
}

}

Q_EXPORT_PLUGIN2( DataViewPlugin, Marble::DataViewPlugin )

#include "DataViewPlugin.moc"
