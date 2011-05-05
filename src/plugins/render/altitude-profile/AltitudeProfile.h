//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ALTITUDEPROFILE_H
#define MARBLE_ALTITUDEPROFILE_H

#include "AbstractFloatItem.h"

namespace Marble {

class GeoDataDocument;
class TileLoader;

class AltitudeProfile : public Marble::AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( AltitudeProfile )
    
public:
    explicit AltitudeProfile(const QPointF& point = QPointF( 10.0, 10.0 ), const QSizeF& size = QSizeF( 150.0, 50.0 ));
    virtual QStringList backendTypes() const;
    virtual bool isInitialized() const;
    virtual void initialize();
    virtual QIcon icon() const;
    virtual QString description() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString name() const;
    void paintContent( GeoPainter *painter, ViewportParams *viewport,
                       const QString& renderPos, GeoSceneLayer * layer = 0 );

private slots:
    void currentRouteChanged( GeoDataDocument* );

private:
    bool m_isInitialized;
    TileLoader *m_tileLoader;
};

}

#endif // MARBLE_ALTITUDEPROFILE_H
