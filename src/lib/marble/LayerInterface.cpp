//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "LayerInterface.h"

#include "RenderState.h"

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

RenderState LayerInterface::renderState() const
{
    return RenderState();
}

QString LayerInterface::runtimeTrace() const
{
    return QString();
}

} // namespace Marble
