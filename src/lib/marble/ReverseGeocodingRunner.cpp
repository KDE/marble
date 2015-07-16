//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012,2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "ReverseGeocodingRunner.h"

namespace Marble
{

ReverseGeocodingRunner::ReverseGeocodingRunner( QObject *parent ) :
    QObject( parent )
{
}

void ReverseGeocodingRunner::setModel( const MarbleModel *model )
{
    m_model = model;
}

const MarbleModel *ReverseGeocodingRunner::model() const
{
    return m_model;
}

}

#include "moc_ReverseGeocodingRunner.cpp"
