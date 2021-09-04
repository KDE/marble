// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

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
