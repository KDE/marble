//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "RoutingRunner.h"

#include "GeoDataPlacemark.h"

#include <QtCore/QString>

namespace Marble
{

RoutingRunner::RoutingRunner( QObject *parent ) :
    QObject( parent ),
    m_model( 0 )
{
}

void RoutingRunner::setModel( MarbleModel *model )
{
    m_model = model;
}

MarbleModel *RoutingRunner::model()
{
    return m_model;
}

}

#include "RoutingRunner.moc"
