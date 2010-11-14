//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//


// Own
#include "DatasetProvider.h"

// Local dir
#include "MarbleDebug.h"
#include "AbstractTile.h"
#include "TileId.h"

namespace Marble
{

class DatasetProviderPrivate
{
 public:
    DatasetProviderPrivate()
    {
    }
};

DatasetProvider::DatasetProvider( QObject *parent )
    : QObject(parent),
      d( new DatasetProviderPrivate() )
{
}

DatasetProvider::~DatasetProvider()
{
    delete d;
}

void DatasetProvider::loadDatasets( AbstractTile * tile )
{
    TileId id = tile->id();

    mDebug() << "DatasetProvider" << id.toString();
}



}

#include "DatasetProvider.moc"
