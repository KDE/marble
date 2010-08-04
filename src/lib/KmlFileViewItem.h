//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#ifndef MARBLE_KMLFILEVIEWITEM_H
#define MARBLE_KMLFILEVIEWITEM_H

#include "GeoDataDocument.h"

namespace Marble
{

class FileManager;

class KmlFileViewItem
{
  public:
    KmlFileViewItem( GeoDataDocument *document );
    ~KmlFileViewItem();

    void saveFile();
    bool isShown() const;
    void setShown( bool value );
    QString name() const;

    GeoDataDocument* document();
  private:
    GeoDataDocument    *m_document;
};

}

#endif
