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

#include "global.h"
#include "MarbleModel.h"


class MarbleDataFacadePrivate
{
 public:
    MarbleDataFacadePrivate( MarbleModel *model )
        : m_model( model )
    {
    }

    MarbleModel  *m_model;
};


MarbleDataFacade::MarbleDataFacade( MarbleModel *model )
    : d( new MarbleDataFacadePrivate( model ) )
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


#include "MarbleDataFacade.moc"
