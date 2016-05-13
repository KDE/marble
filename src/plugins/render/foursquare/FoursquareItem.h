#ifndef FOURSQUAREITEM_H
#define FOURSQUAREITEM_H
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "AbstractDataPluginItem.h"
 
class QFont;
 
namespace Marble
{

class FoursquareItem : public AbstractDataPluginItem
{
    Q_OBJECT

    Q_PROPERTY( QString name READ name NOTIFY nameChanged )
    Q_PROPERTY( QString category READ category NOTIFY categoryChanged )
    Q_PROPERTY( QString address READ address NOTIFY addressChanged )
    Q_PROPERTY( QString city READ city NOTIFY cityChanged )
    Q_PROPERTY( QString country READ country NOTIFY countryChanged )
    Q_PROPERTY( QString usersCount READ usersCount NOTIFY usersCountChanged )
    Q_PROPERTY( QString categoryIconUrl READ categoryIconUrl NOTIFY categoryIconUrlChanged )
    Q_PROPERTY( QString categoryLargeIconUrl READ categoryLargeIconUrl NOTIFY categoryLargeIconUrlChanged )
    
public:
    explicit FoursquareItem( QObject *parent=0 );
 
    ~FoursquareItem();
 
    bool initialized() const;
 
    void paint( QPainter* painter );
 
    bool operator<( const AbstractDataPluginItem *other ) const;
 
    QString name() const;
    
    void setName( const QString& name );
 
    QString category() const;
    
    void setCategory( const QString& category );
 
    QString address() const;
    
    void setAddress( const QString& address );
 
    QString city() const;
    
    void setCity( const QString& city );
 
    QString country() const;
    
    void setCountry( const QString& country );
 
    int usersCount() const;
    
    void setUsersCount( const int count );
    
    QString categoryIconUrl() const;
    
    void setCategoryIconUrl( const QString& url );
    
    QString categoryLargeIconUrl() const;
    
    void setCategoryLargeIconUrl( const QString& url );
 
private:
    QString m_name;
    
    QString m_category;
    
    QString m_address;
    
    QString m_city;
    
    QString m_country;
    
    int m_usersCount;
    
    QString m_categoryIconUrl;
    
    QString m_categoryLargeIconUrl;
 
    static QFont s_font;

Q_SIGNALS:
    void nameChanged();
    
    void categoryChanged();
    
    void addressChanged();
    
    void cityChanged();
    
    void countryChanged();
    
    void usersCountChanged();
    
    void categoryIconUrlChanged();
    
    void categoryLargeIconUrlChanged();
};

}
#endif // FOURSQUAREITEM_H
