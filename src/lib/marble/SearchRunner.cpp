//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012,2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "SearchRunner.h"

namespace Marble
{

SearchRunner::SearchRunner( QObject *parent ) :
    QObject( parent )
{
}

void SearchRunner::setModel( const MarbleModel *model )
{
    m_model = model;
}

const MarbleModel *SearchRunner::model() const
{
    return m_model;
}

}

#include "moc_SearchRunner.cpp"
