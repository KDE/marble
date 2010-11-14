//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_DATASETPROVIDER_H
#define MARBLE_DATASETPROVIDER_H

#include <QtCore/QObject>


namespace Marble
{

class DatasetProviderPrivate;

class AbstractTile;

/**
 * @short The class that provides datasets to the tiles.
 *
 */

class DatasetProvider : public QObject
{
    Q_OBJECT

 public:
    explicit DatasetProvider( QObject *parent = 0 );
    ~DatasetProvider();

 public Q_SLOTS:

    void loadDatasets( AbstractTile * tile ); 

 private:
    Q_DISABLE_COPY( DatasetProvider )

    DatasetProviderPrivate  * const d;
};

}

#endif
