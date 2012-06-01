//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquareItem.h"
#include "GeoPainter.h"
#include "ViewportParams.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>
 
namespace Marble
{

QFont FoursquareItem::s_font = QFont( "Sans Serif", 8 );

FoursquareItem::FoursquareItem(QObject* parent)
    : AbstractDataPluginItem( parent )
{
    setSize( QSize( 0, 0 ) );
}

FoursquareItem::~FoursquareItem()
{
}

QString FoursquareItem::itemType() const
{
    return "foursquareItem";
}

bool FoursquareItem::initialized()
{
    // Find something logical for this
    return true;
}
 
bool FoursquareItem::operator<( const AbstractDataPluginItem *other ) const
{
    const FoursquareItem* item = dynamic_cast<const FoursquareItem*>( other );
    return this->usersCount() < item->usersCount();
}

QString FoursquareItem::name() const
{
    return m_name;
}

void FoursquareItem::setName(const QString& name)
{
    m_name = name;
}

int FoursquareItem::usersCount() const
{
    return m_usersCount;
}

void FoursquareItem::setUsersCount(const int count)
{
    m_usersCount = count;
}

void FoursquareItem::paint( QPainter* painter )
{   
    // Save the old painter state.
    painter->save();
    // We want to paint a black string.
    painter->setPen( QPen( QColor( Qt::black ) ) );
    // We will use our standard font.
    painter->setFont( s_font );
    // Draw the text into the given rect.
    painter->drawText(0, 0, m_name);
    //painter->drawText( QRect( QPoint( 0, 0 ), size().toSize() ), 0, m_name );
    // Restore the old painter state.
    painter->restore();
}

}

#include "FoursquareItem.moc"
