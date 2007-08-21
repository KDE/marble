//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef ABSTRACTFILEVIEWITEM_H
#define ABSTRACTFILEVIEWITEM_H

#include <QtCore/QVariant>

class AbstractFileViewItem
{
  public:
    virtual ~AbstractFileViewItem() {};

    virtual void saveFile() = 0;
    virtual void closeFile() = 0;
    virtual QVariant data() const = 0;
    virtual bool isShown() const = 0;
    virtual void setShown( bool value ) = 0;
};

#endif
