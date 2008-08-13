//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//

#include "AbstractLayerInterface.h"

namespace Marble
{

AbstractLayerInterface::~AbstractLayerInterface()
{
}

bool AbstractLayerInterface::visible() const
{
    return m_visible;
}

void AbstractLayerInterface::setVisible( bool visible )
{
    m_visible = visible;
}

void    AbstractLayerInterface::printToStream( QTextStream &out )const
{
    out << "AbstractLayerInterface Base Class";
}

QTextStream&    operator<<( QTextStream& out,
                            const AbstractLayerInterface &item )
{
    item.printToStream( out );
    return out;
}

}
