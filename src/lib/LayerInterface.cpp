//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
//

#include "LayerInterface.h"

namespace Marble
{

LayerInterface::~LayerInterface()
{
    // nothing to do
}


qreal LayerInterface::zValue() const
{
    return 0.0;
}


} // namespace Marble
