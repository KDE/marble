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

#include "AbstractFileViewItem.h"
#include "GeoDataDocument.h"

namespace Marble
{

class FileManager;

class KmlFileViewItem : public AbstractFileViewItem
{
  public:
    KmlFileViewItem( FileManager &manager, GeoDataDocument *document );
    ~KmlFileViewItem();

    /*
     * AbstractFileViewItem methods
     */
    virtual void saveFile();
    virtual bool isShown() const;
    virtual void setShown( bool value );
    virtual QString name() const;

    GeoDataDocument* document();
  private:
    FileManager  &m_fileManager;
    GeoDataDocument    *m_document;
};

}

#endif
