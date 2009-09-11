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
#include "GeoDataDocument.h"

namespace Marble
{

class PlacemarkManager;

class KmlFileViewItem : public AbstractFileViewItem
{
  public:
    KmlFileViewItem( PlacemarkManager& manager, const GeoDataDocument& document );
    ~KmlFileViewItem();

    /*
     * AbstractFileViewItem methods
     */
    virtual void saveFile();
    virtual void closeFile( int start, bool finalize = true );
    virtual bool isShown() const;
    virtual void setShown( bool value );
    virtual int size() const;
    virtual QString name() const;

    GeoDataDocument* document();
  private:
    PlacemarkManager&   m_placemarkManager;
    GeoDataDocument    m_document;
};

}

#endif
