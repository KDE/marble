//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLStyleSelector.h"
#include <QtCore/QDataStream>

KMLStyleSelector::KMLStyleSelector()
{
}

void KMLStyleSelector::setStyleId( const QString &value )
{
    m_styleId = value;
}

QString KMLStyleSelector::styleId() const
{
    return m_styleId;
}

void KMLStyleSelector::pack( QDataStream& stream ) const
{
    KMLObject::pack( stream );

    stream << m_styleId;
}

void KMLStyleSelector::unpack( QDataStream& stream )
{
    KMLObject::unpack( stream );

    stream >> m_styleId;
}
