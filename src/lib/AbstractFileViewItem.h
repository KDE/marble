//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef ABSTRACTFILEVIEWITEM_H
#define ABSTRACTFILEVIEWITEM_H

#include <QtCore/QVariant>

namespace Marble
{

class AbstractFileViewItem
{
  public:
    enum Roles {
        FilePointerRole = Qt::UserRole + 1
    };
    virtual ~AbstractFileViewItem() {};

    virtual void saveFile() = 0;
    virtual void closeFile( int start, bool finalize = true ) = 0;
    virtual int size() const { return 0; };
    virtual QVariant data( int role = Qt::DisplayRole ) const = 0;
    virtual bool isShown() const = 0;
    virtual void setShown( bool value ) = 0;
};

}

#endif
