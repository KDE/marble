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

#include <QString>
#include <QPixmap>

#include "GeoAprsCoordinates.h"

namespace Marble
{

class ViewportParams;
class GeoPainter;

    class AprsObject
    {

      public:
        AprsObject( const GeoAprsCoordinates &at, const QString &name );
        ~AprsObject();

        void setLocation( const GeoAprsCoordinates &location );
        void setPixmapId( QString &pixmap );
        GeoAprsCoordinates location() const;

        static QColor calculatePaintColor( int from, const QTime &time, int fadetime = 10*60*1000 );
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
