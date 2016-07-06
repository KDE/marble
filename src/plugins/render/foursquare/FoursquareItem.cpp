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

#include <QFontMetrics>
 
namespace Marble
{

QFont FoursquareItem::s_font = QFont( QStringLiteral( "Sans Serif" ), 8 );

FoursquareItem::FoursquareItem(QObject* parent)
    : AbstractDataPluginItem( parent ),
    m_usersCount( 0 )
{
    setSize( QSize( 0, 0 ) );
}

FoursquareItem::~FoursquareItem()
{
}

bool FoursquareItem::initialized() const
{
    // Find something logical for this
    return true;
}
 
bool FoursquareItem::operator<( const AbstractDataPluginItem *other ) const
{
    const FoursquareItem* item = dynamic_cast<const FoursquareItem*>( other );
    return item && this->usersCount() > item->usersCount();
}

QString FoursquareItem::name() const
{
    return m_name;
}

void FoursquareItem::setName(const QString& name)
{
    if( name != m_name ) {
        m_name = name;
        QFontMetrics const fontMetrics( s_font );
        setSize( QSizeF( fontMetrics.width( m_name ) + 10, fontMetrics.height() + 10 ) );
        emit nameChanged();
    }
}

QString FoursquareItem::category() const
{
    return m_category;
}

void FoursquareItem::setCategory(const QString& category)
{
    if( category != m_category ) {
        m_category = category;
        emit categoryChanged();
    }
}

QString FoursquareItem::address() const
{
    return m_address;
}

void FoursquareItem::setAddress(const QString& address)
{
    if( address != m_address ) {
        m_address = address;
        emit addressChanged();
    }
}

QString FoursquareItem::city() const
{
    return m_city;
}
void FoursquareItem::setCity(const QString& city)
{
    if( city != m_city ) {
        m_city = city;
        emit cityChanged();
    }
}

QString FoursquareItem::country() const
{
    return m_country;
}

void FoursquareItem::setCountry(const QString& country)
{
    if( country != m_country ) {
        m_country = country;
        emit countryChanged();
    }
}

int FoursquareItem::usersCount() const
{
    return m_usersCount;
}

void FoursquareItem::setUsersCount(const int count)
{
    if( count != m_usersCount ) {
        m_usersCount = count;
        emit usersCountChanged();
    }
}

QString FoursquareItem::categoryIconUrl() const
{
    return m_categoryIconUrl;
}

void FoursquareItem::setCategoryIconUrl(const QString& url)
{
    if( url != m_categoryIconUrl ) {
        m_categoryIconUrl = url;
        emit categoryIconUrlChanged();
    }
}

QString FoursquareItem::categoryLargeIconUrl() const
{
    return m_categoryLargeIconUrl;
}

void FoursquareItem::setCategoryLargeIconUrl(const QString& url)
{
    if( url != m_categoryLargeIconUrl ) {
        m_categoryLargeIconUrl = url;
        emit categoryLargeIconUrlChanged();
    }
}

void FoursquareItem::paint( QPainter* painter )
{   
    // Save the old painter state.
    painter->save();
    painter->setPen( QPen( QColor( Qt::white ) ) );
    painter->setFont( s_font );
    
    // Draw the text into the given rect.
    QRect rect = QRect( QPoint( 0, 0 ), size().toSize() );
    QRect boundingRect = QRect( QPoint( rect.top(), rect.left() ), QSize( rect.width(), rect.height() ) );
    QPainterPath painterPath;
    painterPath.addRoundedRect( boundingRect, 5, 5 );
    painter->setClipPath( painterPath );
    painter->drawPath( painterPath );
    painter->fillPath( painterPath, QBrush( QColor( "#39AC39" ) ) );
    painter->drawText( rect.adjusted( 5, 5, -5, -5 ), 0, m_name );
    
    painter->restore();
}

}

#include "moc_FoursquareItem.cpp"
