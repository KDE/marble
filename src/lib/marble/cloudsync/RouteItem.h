//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef ROUTEITEM_H
#define ROUTEITEM_H

class QString;
class QUrl;
class QIcon;

namespace Marble {

class RouteItem {

public:
    RouteItem();
    RouteItem( const RouteItem &other );
    ~RouteItem();

    RouteItem& operator=(const RouteItem &other);
    bool operator==( const RouteItem &other ) const;

    QString identifier() const;
    void setIdentifier( const QString &identifier );

    QString name() const;
    void setName( const QString &name );

    QIcon preview() const;
    void setPreview(const QIcon &preview );

    QUrl previewUrl() const;
    void setPreviewUrl( const QUrl &previewUrl );

    QString distance() const;
    void setDistance( const QString &distance );

    QString duration() const;
    void setDuration( const QString &duration );

    bool onCloud() const;
    void setOnCloud( const bool onCloud );

private:
    class Private;
    Private *d;
};

}

#endif // ROUTEITEM_H
