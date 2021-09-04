// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_GEODATAOVERLAY_H
#define MARBLE_GEODATAOVERLAY_H

#include "geodata_export.h"
#include "GeoDataFeature.h"

class QColor;
class QImage;

namespace Marble
{

class GeoDataOverlayPrivate;

/**
 */
class GEODATA_EXPORT GeoDataOverlay: public GeoDataFeature
{
 public:
    /** Constructor */
    GeoDataOverlay();

    ~GeoDataOverlay() override;

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
      * is part of (@see fileName of GeoDataDocument)
      */
    QString absoluteIconFile() const;

protected:
    explicit GeoDataOverlay(GeoDataOverlayPrivate *priv);
    GeoDataOverlay(const GeoDataOverlay& other, GeoDataOverlayPrivate *priv);

    bool equals(const GeoDataOverlay &other) const;
    using GeoDataFeature::equals;

 private:
    Q_DECLARE_PRIVATE(GeoDataOverlay)
};

}

#endif
