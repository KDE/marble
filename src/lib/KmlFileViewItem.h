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

namespace Marble
{

class GeoDataDocument;
class PlaceMarkManager;

class KmlFileViewItem : public AbstractFileViewItem
{
  public:
    KmlFileViewItem( PlaceMarkManager& manager, GeoDataDocument& document );
    ~KmlFileViewItem();

    /*
     * AbstractFileViewItem methods
     */
    virtual void saveFile();
    virtual void closeFile();
    virtual QVariant data( int role = Qt::DisplayRole ) const;
    virtual bool isShown() const;
    virtual void setShown( bool value );

  private:
    PlaceMarkManager&   m_placeMarkManager;
    GeoDataDocument&        m_document;
};

}

#endif
