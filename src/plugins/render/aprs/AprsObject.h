//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#ifndef APRSOBJECT_H
#define APRSOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "GeoAprsCoordinates.h"
#include "GeoPainter.h"

namespace Marble
{

    class AprsObject
    {

      public:
        AprsObject();
        AprsObject( const GeoAprsCoordinates &at, QString &name );
        AprsObject( const qreal &lon, const qreal &lat, const QString &name,
                    int where = GeoAprsCoordinates::FromNowhere );
        ~AprsObject();

        void setLocation( GeoAprsCoordinates location );
        void setLocation( qreal lon, qreal lat, int from );
        void setPixmapId( QString &pixmap );
        void setSeenFrom( int where );
        GeoAprsCoordinates location();

        QColor calculatePaintColor( GeoPainter *painter ) const;
        QColor calculatePaintColor( GeoPainter *painter, int from,
                                    const QTime &time,
                                    int fadetime = 10*60*1000 ) const;
        void render( GeoPainter *painter, ViewportParams *viewport,
                     int fadeTime = 10*60, int hideTime = 30*60 );

      private:
        QList<GeoAprsCoordinates>     m_history;
        QString                       m_myName;
        int                           m_seenFrom;
        bool                          m_havePixmap;
        QString                       m_pixmapFilename;
        QPixmap                       *m_pixmap;
    };

}

#endif /* APRSOBJECT_H */
