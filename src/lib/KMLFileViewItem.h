//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLFILEVIEWITEM_H
#define KMLFILEVIEWITEM_H

#include <QtCore/QVariant>
#include "AbstractFileViewItem.h"

class GeoDataDocument;
class PlaceMarkManager;

class KMLFileViewItem : public AbstractFileViewItem
{
  public:
    KMLFileViewItem( PlaceMarkManager& manager, GeoDataDocument& document );
    ~KMLFileViewItem();

    /*
     * AbstractFileViewItem methods
     */
    virtual void saveFile();
    virtual void closeFile();
    virtual QVariant data() const;
    virtual bool isShown() const;
    virtual void setShown( bool value );

  private:
    PlaceMarkManager&   m_placeMarkManager;
    GeoDataDocument&        m_document;
};

#endif
