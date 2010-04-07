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

#ifndef MARBLE_ABSTRACTFILEVIEWITEM_H
#define MARBLE_ABSTRACTFILEVIEWITEM_H

#include <QtCore/QString>

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
    virtual bool isShown() const = 0;
    virtual void setShown( bool value ) = 0;
    virtual QString name() const = 0;
};

}

#endif
