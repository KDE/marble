//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "AbstractRoutingProvider.h"

namespace Marble {

AbstractRoutingProvider::AbstractRoutingProvider(QObject *parent) :
        QObject(parent)
{
    // nothing to do
}

AbstractRoutingProvider::~AbstractRoutingProvider()
{
    // nothing to do
}

} // namespace Marble

#include "AbstractRoutingProvider.moc"
