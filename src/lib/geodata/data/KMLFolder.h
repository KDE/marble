//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#ifndef GEODATAFOLDER_H
#define GEODATAFOLDER_H

#include <QtCore/QVector>
#include "GeoDataContainer.h"

class GeoDataFolder : public GeoDataContainer
{
 public:
    GeoDataFolder();
    ~GeoDataFolder();

    void addFolder( GeoDataFolder* folder );

 protected:
    QVector <GeoDataFolder*> m_folderVector;
};

#endif // GEODATAFOLDER_H
