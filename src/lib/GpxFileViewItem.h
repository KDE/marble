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

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include "AbstractFileViewItem.h"

namespace Marble
{

class GpxFile;

class GpxFileViewItem
  : public QObject,
    public AbstractFileViewItem
{
  public:
    /// Takes ownership of file
    explicit GpxFileViewItem( GpxFile* file );
    ~GpxFileViewItem();

    /*
     * AbstractFileViewItem methods
     */
    virtual void saveFile();
    virtual void closeFile( int start, bool finalize = true );
    virtual QVariant data( int role = Qt::DisplayRole ) const;
    virtual bool isShown() const;
    virtual void setShown( bool value );

  private:
    Q_DISABLE_COPY( GpxFileViewItem )
    GpxFile*    m_gpxFile;
};

}

#endif
