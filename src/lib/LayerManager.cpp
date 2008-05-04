//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "LayerManager.h"

// Qt

// Local dir

LayerManager::LayerManager(QObject *parent)
    : QObject(parent)
{
}

LayerManager::~LayerManager()
{
}

void LayerManager::loadLayers()
{
}

#include "LayerManager.moc"
