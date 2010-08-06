//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_SERVERLAYOUT_H
#define MARBLE_SERVERLAYOUT_H

#include <QUrl>

namespace Marble
{
class GeoSceneTexture;
class TileId;

class ServerLayout
{
public:
    ServerLayout( GeoSceneTexture *textureLayer );
    virtual ~ServerLayout();

    /**
     * Translates given tile @p id using a @p prototypeUrl into an URL
     * that can be used for downloading.
     *
     * @param prototypeUrl prototype URL, to be completed by this method
     * @param id Marble-specific ID of requested tile
     * @return completed URL for requested tile id
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const = 0;

protected:
    qint64 numTilesX( const Marble::TileId &tileId ) const;
    qint64 numTilesY( const Marble::TileId &tileId ) const;

protected:
    GeoSceneTexture *const m_textureLayer;
};

class MarbleServerLayout : public ServerLayout
{
public:
    explicit MarbleServerLayout( GeoSceneTexture *textureLayer );

    /**
     * Completes the path of the @p prototypeUrl and returns it.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;
};

class OsmServerLayout : public ServerLayout
{
public:
    explicit OsmServerLayout( GeoSceneTexture *textureLayer );

    /**
     * Appends %zoomLevel/%x/%y.%suffix to the path of the @p prototypeUrl and returns
     * the result.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;
};

class CustomServerLayout : public ServerLayout
{
public:
    CustomServerLayout( GeoSceneTexture *texture );

    /**
     * Replaces escape sequences in the @p prototypeUrl by the values in @p id
     * and returns the result.
     *
     * Escape sequences are: {zoomLevel}, {x}, and {y}.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const;
};

class WmsServerLayout : public ServerLayout
{
public:
    WmsServerLayout( GeoSceneTexture *texture );

    /**
     * Adds WMS query items to the @p prototypeUrl and returns the result.
     *
     * The following items are added: service, request, version, width, height, bbox.
     *
     * The following items are only added if they are not already specified in the dgml file:
     * styles, format, srs, layers.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const;

private:
    qreal latBottom( const Marble::TileId &tileId ) const;
    qreal latTop( const Marble::TileId &tileId ) const;
    QString epsgCode() const;
};

}

#endif
