//
// This file is part of the Marble Virtual Globe.
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
#include <QtGui/QSortFilterProxyModel>

#include "global.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "Planet.h"

#include "FileViewModel.h"
#include "PluginManager.h"
#include "GeoDataTreeModel.h"
#include "GeoDataParser.h"
#include "PositionTracking.h"

#include "GeoDataTypes.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

#include "routing/RoutingManager.h"

namespace Marble
{

class MarbleDataFacadePrivate
{
 public:
    MarbleDataFacadePrivate( MarbleModel *model )
        : m_model( model )
    {
    }

    MarbleModel  *const m_model;
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
    return d->m_model->clock()->dateTime();
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

QAbstractItemModel* MarbleDataFacade::placemarkModel()
{
    return d->m_model->placemarkModel();
}

FileViewModel* MarbleDataFacade::fileViewModel() const
{
    return d->m_model->fileViewModel();
}

PluginManager* MarbleDataFacade::pluginManager()
{
    return d->m_model->pluginManager();
}

RoutingManager* MarbleDataFacade::routingManager()
{
    return d->m_model->routingManager();
}

QAbstractItemModel* MarbleDataFacade::treeModel() const
{
    return d->m_model->treeModel();
}

}

