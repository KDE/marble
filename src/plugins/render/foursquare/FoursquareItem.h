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
    
public:
    explicit FoursquareItem( QObject *parent=0 );
 
    ~FoursquareItem();
 
    QString itemType() const;
 
    bool initialized();
 
    void paint( QPainter* painter );
 
    bool operator<( const AbstractDataPluginItem *other ) const;
 
    QString name() const;
    
    void setName( const QString& name );
 
    int usersCount() const;
    
    void setUsersCount( const int count );
 
 private:
    QString m_name;
    int m_usersCount;
 
    static QFont s_font;
};

}
#endif // FOURSQUAREITEM_H
