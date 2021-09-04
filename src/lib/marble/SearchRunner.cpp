// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012, 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

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
