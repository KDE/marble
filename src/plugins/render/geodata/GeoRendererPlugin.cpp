//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "GeoRendererPlugin.h"
#include "GeoRendererView.h"

#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"
#include "GeoDataTreeModel.h"
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


namespace Marble
{

GeoRendererPlugin::GeoRendererPlugin()
    : m_view( 0 ),
      m_isInitialized( false )
{
}

GeoRendererPlugin::~GeoRendererPlugin()
{
    delete( m_view );
}

QStringList GeoRendererPlugin::backendTypes() const
{
    return QStringList( "geodata" );
}

QString GeoRendererPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList GeoRendererPlugin::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

QString GeoRendererPlugin::name() const
{
    return tr( "GeoRenderer Plugin" );
}

QString GeoRendererPlugin::guiString() const
{
    return tr( "&GeoRenderer Plugin" );
}

QString GeoRendererPlugin::nameId() const
{
    return QString( "GeoRenderer-plugin" );
}

QString GeoRendererPlugin::description() const
{
    return tr( "This plugin draws the map content (e.g. provided via a KML file)." );
}

QIcon GeoRendererPlugin::icon () const
{
    return QIcon();
}


void GeoRendererPlugin::initialize ()
{
    m_view = new GeoRendererView();
    m_isInitialized = true;
}

bool GeoRendererPlugin::isInitialized () const
{
    return m_isInitialized;
}

bool GeoRendererPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                                const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( viewport )
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    if ( renderPos != "HOVERS_ABOVE_SURFACE" ) {
        return true;
    }

    if( !dataFacade() || !dataFacade()->treeModel() )
        return false;

    if( !m_view->model() ) m_view->setModel( dataFacade()->treeModel() );
    m_view->setGeoPainter( painter );
    return true;
}

}

Q_EXPORT_PLUGIN2( GeoRendererPlugin, Marble::GeoRendererPlugin )

#include "GeoRendererPlugin.moc"
