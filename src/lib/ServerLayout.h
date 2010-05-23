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
};

class MarbleServerLayout : public ServerLayout
{
public:
    explicit MarbleServerLayout( GeoSceneTexture *textureLayer );

    /**
     * Completes the path of the @p prototypeUrl and returns it.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId & ) const;

private:
    GeoSceneTexture *const m_textureLayer;
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

private:
    GeoSceneTexture *const m_textureLayer;
};

class CustomServerLayout : public ServerLayout
{
public:
    /**
     * Replaces escape sequences in the @p prototypeUrl by the values in @p id
     * and returns the result.
     *
     * Escape sequences are: {zoomLevel}, {x}, and {y}.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const TileId &id ) const;
};

class LatLonBoxServerLayout : public ServerLayout
{
public:
    /**
     * Replaces escape sequences in the @p prototypeUrl according to the lat-lon box
     * covered by the given @p tileId and returns the result.
     *
     * Escape sequences are: {latTop}, {latBottom}, {lonLeft}, and {lonRight}.
     */
    virtual QUrl downloadUrl( const QUrl &prototypeUrl, const Marble::TileId &tileId ) const;
};

}

#endif
