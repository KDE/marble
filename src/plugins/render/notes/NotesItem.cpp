// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesItem.h"
#include "MarbleDirs.h"

#include <QPainter>
#include <QRect>

using namespace Marble;

NotesItem::NotesItem(QObject *parent)
    : AbstractDataPluginItem(parent),
      m_pixmap(MarbleDirs::path("bitmaps/waypoint.png"))
{
    setSize(m_pixmap.size());
    setCacheMode(ItemCoordinateCache);
}

NotesItem::~NotesItem()
{
}

bool NotesItem::initialized() const
{
    return !id().isEmpty();
}

bool NotesItem::operator<(const AbstractDataPluginItem *other) const
{
    return this->id() < other->id();
}

void NotesItem::paint(QPainter *painter)
{
    painter->drawPixmap(0, 0, m_pixmap);
}
