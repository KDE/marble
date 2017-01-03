//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Spencer Brown <spencerbrown991@gmail.com>
//

#ifndef NOTESITEM_H
#define NOTESITEM_H

#include "AbstractDataPluginItem.h"
#include <QPixmap>

namespace Marble
{

class NotesItem : public AbstractDataPluginItem
{
    Q_OBJECT

 public:
    explicit NotesItem( QObject *parent );

    ~NotesItem() override;

    bool initialized() const override;

    void paint( QPainter *painter ) override;

    bool operator<( const AbstractDataPluginItem *other ) const override;

private:
    static const QFont s_font;
    static const int s_labelOutlineWidth;
    QPixmap m_pixmap;
};

}
#endif // NOTESITEM_H
