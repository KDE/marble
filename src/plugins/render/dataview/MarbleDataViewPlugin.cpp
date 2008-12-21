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

#include "MarbleDataViewPlugin.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtGui/QTreeView>

#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "MarbleGeoDataModel.h"
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

#include "MarbleGeoDataDebugModel.h"
#include "modeltest.h"

namespace Marble
{

MarbleDataViewPlugin::MarbleDataViewPlugin()
: m_dataView(NULL), m_debugModel(NULL)
{
}

MarbleDataViewPlugin::~MarbleDataViewPlugin()
{
    if( m_dataView )
        delete m_dataView;
    if( m_debugModel )
        delete m_debugModel;
}

QStringList MarbleDataViewPlugin::backendTypes() const
{
    return QStringList( "debug" );
}

QString MarbleDataViewPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MarbleDataViewPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString MarbleDataViewPlugin::name() const
{
    return tr( "DataView Plugin" );
}

QString MarbleDataViewPlugin::guiString() const
{
    return tr( "&DataView Plugin" );
}

QString MarbleDataViewPlugin::nameId() const
{
    return QString( "DataView-plugin" );
}

QString MarbleDataViewPlugin::description() const
{
    return tr( "A plugin that displays all the loaded data in a separate window." );
}

QIcon MarbleDataViewPlugin::icon () const
{
    return QIcon();
}


void MarbleDataViewPlugin::initialize ()
{
    m_dataView = new QTreeView;
    m_debugModel = new MarbleGeoDataDebugModel( dataFacade()->geoDataModel()->geoDataRoot() );
    
    m_dataView->setModel( m_debugModel );
    m_dataView->show();
    
    connect(dataFacade()->geoDataModel(), SIGNAL( dataChanged() ), m_debugModel, SLOT(update()));
   // new ModelTest( m_debugModel );
}

bool MarbleDataViewPlugin::isInitialized () const
{
    return true;
}

bool MarbleDataViewPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    return true;
}

}

Q_EXPORT_PLUGIN2(MarbleDataViewPlugin, Marble::MarbleDataViewPlugin)

#include "MarbleDataViewPlugin.moc"
