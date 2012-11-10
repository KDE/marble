//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Anton Chernov <chernov.anton.mail@gmail.com>
// Copyright 2012      "LOTES TM" LLC <lotes.sis@gmail.com>
//

#include "DeclarativeDataPluginItem.h"

namespace Marble {

DeclarativeDataPluginItem::DeclarativeDataPluginItem( QObject *parent )
    : AbstractDataPluginItem( parent )
{
    // nothing to do
}

DeclarativeDataPluginItem::~DeclarativeDataPluginItem()
{
    // nothing to do
}

QString DeclarativeDataPluginItem::itemType() const
{
    return "DeclarativeDataPluginItem";
}

bool DeclarativeDataPluginItem::initialized()
{
    return true;
}

bool DeclarativeDataPluginItem::operator<( const AbstractDataPluginItem *other ) const
{
    return other->id()< id() ;
}

}

#include "DeclarativeDataPluginItem.moc"
