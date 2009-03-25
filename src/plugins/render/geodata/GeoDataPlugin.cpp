//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "GeoDataPlugin.h"

#include <QtCore/QDebug>

#include "MarbleDataFacade.h"
#include "MarbleGeoDataModel.h"
#include "GeoDataView.h"
#include "GeoPainter.h"


namespace Marble
{

GeoDataPlugin::~GeoDataPlugin()
{
    delete( m_view );
}

QStringList GeoDataPlugin::backendTypes() const
{
    return QStringList( "geodata" );
}

QString GeoDataPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList GeoDataPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString GeoDataPlugin::name() const
{
    return tr( "GeoData Plugin" );
}

QString GeoDataPlugin::guiString() const
{
    return tr( "&GeoData Plugin" );
}

QString GeoDataPlugin::nameId() const
{
    return QString( "GeoData-plugin" );
}

QString GeoDataPlugin::description() const
{
    return tr( "This is a simple test plugin." );
}

QIcon GeoDataPlugin::icon () const
{
    return QIcon();
}


void GeoDataPlugin::initialize ()
{
    m_view = new GeoDataView();
}

bool GeoDataPlugin::isInitialized () const
{
    return true;
}

bool GeoDataPlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    if( !dataFacade() || !dataFacade()->renderModel() )
        return false;

    if( !m_view->model() ) m_view->setModel( dataFacade()->renderModel() );
    m_view->setGeoPainter( painter );
    return true;
}

}

Q_EXPORT_PLUGIN2( GeoDataPlugin, Marble::GeoDataPlugin )

#include "GeoDataPlugin.moc"
