// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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
