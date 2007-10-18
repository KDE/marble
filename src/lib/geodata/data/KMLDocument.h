//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GEODATADOCUMENT_H
#define GEODATADOCUMENT_H

#include <QtCore/QHash>

#include "GeoDataFolder.h"

class QIODevice;
class GeoDataStyle;

class GeoDataDocument : public GeoDataFolder
{
  public:
    GeoDataDocument();
    ~GeoDataDocument();

    void load( QIODevice& source );

    void addStyle( GeoDataStyle* style );
    const GeoDataStyle& getStyle( QString styleId ) const;

  private:
    QHash < QString, GeoDataStyle* > m_styleHash;
};

#endif // GEODATADOCUMENT_H
