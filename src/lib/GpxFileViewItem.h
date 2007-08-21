//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef GPXFILEVIEWITEM_H
#define GPXFILEVIEWITEM_H

#include <QObject>
#include "AbstractFileViewItem.h"

class GpxFile;

class GpxFileViewItem
  : public QObject,
    public AbstractFileViewItem
{
  public:
    GpxFileViewItem( GpxFile* file );
    ~GpxFileViewItem();

    virtual void saveFile();
    virtual void closeFile();
    virtual QVariant data() const;
    virtual bool isShown() const;
    virtual void setShown( bool value );

  private:
    GpxFile*    m_gpxFile;
};

#endif
