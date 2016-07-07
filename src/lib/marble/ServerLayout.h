//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010,2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_SERVERLAYOUT_H
#define MARBLE_SERVERLAYOUT_H

class QUrl;
class QString;

namespace Marble
{
class GeoSceneTileDataset;
class TileId;

class ServerLayout
{
public:
    explicit ServerLayout( GeoSceneTileDataset *textureLayer );
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

    /**
     * Returns the name of the server layout to be used as the value in the
     * mode attribute in the DGML file.
     */
    virtual QString name() const = 0;

    /**
     * Returns the sourceDir of the texture layer, or an empty string if the texture layer is 0
     */
    QString sourceDir() const;

protected:
    GeoSceneTileDataset *const m_textureLayer;
};

class MarbleServerLayout : public ServerLayout
{
public:
    explicit MarbleServerLayout( GeoSceneTileDataset *textureLayer );

    /**
     * Completes the path of the @p prototypeUrl and returns it.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;

    virtual QString name() const;
};

class OsmServerLayout : public ServerLayout
{
public:
    explicit OsmServerLayout( GeoSceneTileDataset *textureLayer );

    /**
     * Appends %zoomLevel/%x/%y.%suffix to the path of the @p prototypeUrl and returns
     * the result.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;

    virtual QString name() const;
};

class CustomServerLayout : public ServerLayout
{
public:
    explicit CustomServerLayout( GeoSceneTileDataset *texture );

    /**
     * Replaces escape sequences in the @p prototypeUrl by the values in @p id
     * and returns the result.
     *
     * Escape sequences are: {zoomLevel}, {x}, and {y}.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const;

    virtual QString name() const;
};

class WmsServerLayout : public ServerLayout
{
public:
    explicit WmsServerLayout( GeoSceneTileDataset *texture );

    /**
     * Adds WMS query items to the @p prototypeUrl and returns the result.
     *
     * The following items are added: service, request, version, width, height, bbox.
     *
     * The following items are only added if they are not already specified in the dgml file:
     * styles, format, srs, layers.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const;

    virtual QString name() const;

    QString epsgCode() const;
};

class QuadTreeServerLayout : public ServerLayout
{
public:
    explicit QuadTreeServerLayout( GeoSceneTileDataset* textureLayer );
    virtual QUrl downloadUrl( const QUrl &, const Marble::TileId & ) const;

    virtual QString name() const;

private:
    static QString encodeQuadTree( const Marble::TileId & );
};

class TmsServerLayout : public ServerLayout
{
public:
    explicit TmsServerLayout( GeoSceneTileDataset *textureLayer );

    /**
     * Appends %zoomLevel/%x/2^%zoomLevel-%y-1.%suffix to the path of the @p prototypeUrl and returns
     * the result.
     * TMS (TileMapService) maps take the origin for y coordinate at the bottom of the map,
     * as opposed to what Marble and OpenStreepMap (SlippyTiles) do.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;

    virtual QString name() const;
};

}

#endif
