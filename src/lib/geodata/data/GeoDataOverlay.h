//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_GEODATAOVERLAY_H
#define MARBLE_GEODATAOVERLAY_H

#include "geodata_export.h"
#include "GeoDataFeature.h"

#include <QtCore/QVector>
#include <QtGui/QColor>
#include <QtGui/QImage>

namespace Marble
{

class GeoDataOverlayPrivate;

class GEODATA_EXPORT GeoDataOverlay: public GeoDataFeature
{
 public:
    /** Constructor */
    GeoDataOverlay();

    ~GeoDataOverlay();

    GeoDataOverlay( const GeoDataOverlay &other );

    GeoDataOverlay& operator=( const GeoDataOverlay &other );

    QColor color() const;

    void setColor( const QColor &color );

    int drawOrder() const;

    void setDrawOrder( int order );

    QImage icon() const;

    void setIcon( const QImage &icon );

    void setIconFile( const QString &path );

    QString iconFile() const;

    /**
      * Returns the #iconFile as an absolute filename. Relative files are
      * resolved relative to the directory of the GeoDataDocument this overlay
      * is part of (see #fileName of #GeoDataDocument)
      */
    QString absoluteIconFile() const;

private:
    GeoDataOverlayPrivate * const d;
};

}

#endif
