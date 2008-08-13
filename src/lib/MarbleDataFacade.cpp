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

#include "global.h"
#include "MarbleModel.h"
#include "MarbleGeoDataModel.h"
#include "GeoDataDocument.h"
#include "GeoDataParser.h"

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

QDateTime MarbleDataFacade::dateTime() const
{
    return d->m_model->dateTime()->datetime();
}

MarbleGeoDataModel* MarbleDataFacade::geoDataModel()
{
    return d->m_geodatamodel;
}

}

#include "MarbleDataFacade.moc"
