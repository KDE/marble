//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleDataFacade.h"

#include <cmath>

// just for now:
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QAbstractItemModel>

#include "global.h"
#include "MarbleModel.h"
#include "MarbleGeoDataModel.h"
#include "Planet.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

namespace Marble
{

class MarbleDataFacadePrivate
{
 public:
    MarbleDataFacadePrivate( MarbleModel *model, MarbleGeoDataModel *gmodel )
        : m_model( model ), m_geodatamodel( gmodel )
    {
    }

    ~MarbleDataFacadePrivate()
    {
        delete m_geodatamodel;
    }

    MarbleModel  *m_model;
    MarbleGeoDataModel *m_geodatamodel;
};


MarbleDataFacade::MarbleDataFacade( MarbleModel *model )
    : d( new MarbleDataFacadePrivate( model, new MarbleGeoDataModel() ) )
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

MarbleGeoDataModel* MarbleDataFacade::geoDataModel()
{
    return d->m_geodatamodel;
}

QAbstractItemModel* MarbleDataFacade::renderModel()
{
    return d->m_model->geometryModel();
}

FileViewModel* MarbleDataFacade::fileViewModel() const
{
    if(d->m_model)
        return d->m_model->fileViewModel();
    else
        return 0;
}

}

