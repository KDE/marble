//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleGeoDataPlugin.h"

#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>

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


namespace Marble
{

MarbleGeoDataPlugin::~MarbleGeoDataPlugin()
{
    delete( m_view );
}

QStringList MarbleGeoDataPlugin::backendTypes() const
{
    return QStringList( "geodata" );
}

QString MarbleGeoDataPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MarbleGeoDataPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString MarbleGeoDataPlugin::name() const
{
    return tr( "GeoData Plugin" );
}

QString MarbleGeoDataPlugin::guiString() const
{
    return tr( "&GeoData Plugin" );
}

QString MarbleGeoDataPlugin::nameId() const
{
    return QString( "GeoData-plugin" );
}

QString MarbleGeoDataPlugin::description() const
{
    return tr( "This is a simple test plugin." );
}

QIcon MarbleGeoDataPlugin::icon () const
{
    return QIcon();
}


void MarbleGeoDataPlugin::initialize ()
{
    m_view = new MarbleGeoDataView();
}

bool MarbleGeoDataPlugin::isInitialized () const
{
    return true;
}

bool MarbleGeoDataPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if( !dataFacade() || !dataFacade()->renderModel() )
        return false;

    if( !m_view->model() ) m_view->setModel( dataFacade()->renderModel() );
    m_view->setGeoPainter( painter );
    return true;
}

}

Q_EXPORT_PLUGIN2( MarbleGeoDataPlugin, Marble::MarbleGeoDataPlugin )

#include "MarbleGeoDataPlugin.moc"
