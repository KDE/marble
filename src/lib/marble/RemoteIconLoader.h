//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_REMOTEICONLOADER_H
#define MARBLE_REMOTEICONLOADER_H

#include <QObject>

class QString;
class QByteArray;
class QImage;
class QUrl;

namespace Marble
{
class RemoteIconLoaderPrivate;

class RemoteIconLoader : public QObject
{
    Q_OBJECT;
public:
    explicit RemoteIconLoader( QObject *parent = 0 );
    ~RemoteIconLoader();

    /**
        * Handles the icon request for passed url( whether icon is available
        * in cache or on disk or it should gor for a download
        */
    QImage load( const QUrl& url );

Q_SIGNALS:
    /**
        * Signal to indicate that image has been downloaded
        * and is ready to be display now
        */
    void iconReady();

private Q_SLOTS:
    void storeIcon( const QByteArray&, const QString& );

private:
    RemoteIconLoaderPrivate *d;
};

}

#endif  // MARBLE_REMOTEICONLOADER_H
