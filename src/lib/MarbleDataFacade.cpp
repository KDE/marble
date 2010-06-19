//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>
//

#include "MarbleDataFacade.h"

#include <cmath>

// just for now:
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QAbstractItemModel>

#include "global.h"
#include "ExtDateTime.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleGeoDataModel.h"
#include "MarbleGeometryModel.h"
#include "MarblePlacemarkModel.h"
#include "Planet.h"

#include "FileViewModel.h"
#include "PluginManager.h"
#include "GeoDataParser.h"

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

namespace Marble
{

class MarbleDataFacadePrivate
{
 public:
    MarbleDataFacadePrivate( MarbleModel *model )
        : m_model( model ),
        m_fileviewmodel( new FileViewModel() ),
        m_geodatamodel( new MarbleGeoDataModel() ),
        m_geometrymodel( new MarbleGeometryModel() ),
        m_placemarkmodel( new MarblePlacemarkModel )
    {
    }

    ~MarbleDataFacadePrivate()
    {
        delete m_fileviewmodel;
        delete m_geodatamodel;
        delete m_geometrymodel;
        delete m_placemarkmodel;
    }

    MarbleModel  *m_model;
    FileViewModel *m_fileviewmodel;
    MarbleGeoDataModel *m_geodatamodel;
    MarbleGeometryModel *m_geometrymodel;
    MarblePlacemarkModel *m_placemarkmodel;
};


MarbleDataFacade::MarbleDataFacade( MarbleModel *model )
    : d( new MarbleDataFacadePrivate( model ) )
{
}

MarbleDataFacade::~MarbleDataFacade()
{
    delete d;
}

qreal MarbleDataFacade::planetRadius() const
{
    return d->m_model->planet()->radius();
}

const Planet* MarbleDataFacade::planet() const
{
    return d->m_model->planet();
}

QDateTime MarbleDataFacade::dateTime() const
{
    return d->m_model->dateTime()->datetime();
}

QString MarbleDataFacade::target() const
{
    GeoSceneDocument *mapTheme = d->m_model->mapTheme();
    if ( mapTheme )
    {
        return mapTheme->head()->target();
    }

    return QString();
}

PositionTracking* MarbleDataFacade::positionTracking() const
{
    return d->m_model->positionTracking();
}

MarbleGeoDataModel* MarbleDataFacade::geoDataModel()
{
    return d->m_geodatamodel;
}

MarbleGeometryModel* MarbleDataFacade::geometryModel()
{
    return d->m_geometrymodel;
}

MarblePlacemarkModel* MarbleDataFacade::placemarkModel()
{
    return d->m_placemarkmodel;
}

FileViewModel* MarbleDataFacade::fileViewModel() const
{
    return d->m_fileviewmodel;
}

PluginManager* MarbleDataFacade::pluginManager()
{
    return d->m_model->pluginManager();
}

}

