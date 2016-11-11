//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoGraphicsScene.h"

#include "GeoDataFeature.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataTypes.h"
#include "GeoGraphicsItem.h"
#include "TileId.h"
#include "TileCoordsPyramid.h"
#include "MarbleDebug.h"

#include <QMap>
#include <QRect>

namespace Marble
{

class GeoGraphicsScenePrivate
{
public:
    GeoGraphicsScene *q;
    explicit GeoGraphicsScenePrivate(GeoGraphicsScene *parent) :
        q(parent)
    {
    }

    ~GeoGraphicsScenePrivate()
    {
        q->clear();
    }

    QMap<TileId, QList<GeoGraphicsItem*> > m_items;
    QMultiHash<const GeoDataFeature*, TileId> m_features;

    // Stores the items which have been clicked;
    QList<GeoGraphicsItem*> m_selectedItems;

    GeoDataStyle::Ptr highlightStyle(const GeoDataDocument *document, const GeoDataStyleMap &styleMap);

    void selectItem( GeoGraphicsItem *item );
    void applyHighlightStyle(GeoGraphicsItem *item, const GeoDataStyle::Ptr &style );
};

GeoDataStyle::Ptr GeoGraphicsScenePrivate::highlightStyle( const GeoDataDocument *document,
                                                       const GeoDataStyleMap &styleMap )
{
    // @todo Consider QUrl parsing when external styles are suppported
    QString highlightStyleId = styleMap.value(QStringLiteral("highlight"));
    highlightStyleId.remove(QLatin1Char('#'));
    if ( !highlightStyleId.isEmpty() ) {
        GeoDataStyle::Ptr highlightStyle(new GeoDataStyle( *document->style(highlightStyleId) ));
        return highlightStyle;
    }
    else {
        return GeoDataStyle::Ptr();
    }
}

void GeoGraphicsScenePrivate::selectItem( GeoGraphicsItem* item )
{
    m_selectedItems.append( item );
}

void GeoGraphicsScenePrivate::applyHighlightStyle(GeoGraphicsItem* item, const GeoDataStyle::Ptr &highlightStyle )
{
    item->setHighlightStyle( highlightStyle );
    item->setHighlighted( true );
}

GeoGraphicsScene::GeoGraphicsScene( QObject* parent ):
    QObject( parent ),
    d( new GeoGraphicsScenePrivate(this) )
{

}

GeoGraphicsScene::~GeoGraphicsScene()
{
    delete d;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::items( const GeoDataLatLonBox &box, int zoomLevel ) const
{
    if ( box.west() > box.east() ) {
        // Handle boxes crossing the IDL by splitting it into two separate boxes
        GeoDataLatLonBox left;
        left.setWest( -M_PI );
        left.setEast( box.east() );
        left.setNorth( box.north() );
        left.setSouth( box.south() );

        GeoDataLatLonBox right;
        right.setWest( box.west() );
        right.setEast( M_PI );
        right.setNorth( box.north() );
        right.setSouth( box.south() );

        return items(left, zoomLevel) + items(right, zoomLevel);
    }

    QList< GeoGraphicsItem* > result;
    QRect rect;
    qreal north, south, east, west;
    box.boundaries( north, south, east, west );
    TileId key;

    key = TileId::fromCoordinates( GeoDataCoordinates(west, north, 0), zoomLevel );
    rect.setLeft( key.x() );
    rect.setTop( key.y() );

    key = TileId::fromCoordinates( GeoDataCoordinates(east, south, 0), zoomLevel );
    rect.setRight( key.x() );
    rect.setBottom( key.y() );
    
    TileCoordsPyramid pyramid( 0, zoomLevel );
    pyramid.setBottomLevelCoords( rect );

    for ( int level = pyramid.topLevel(); level <= pyramid.bottomLevel(); ++level ) {
        QRect const coords = pyramid.coords( level );
        int x1, y1, x2, y2;
        coords.getCoords( &x1, &y1, &x2, &y2 );
        for ( int x = x1; x <= x2; ++x ) {
            bool const isBorderX = x == x1 || x == x2;
            for ( int y = y1; y <= y2; ++y ) {
                bool const isBorder = isBorderX || y == y1 || y == y2;
                const TileId tileId = TileId( 0, level, x, y );
                foreach(GeoGraphicsItem *object, d->m_items.value( tileId )) {
                    if (object->minZoomLevel() <= zoomLevel && object->visible()) {
                        if (!isBorder || object->latLonAltBox().intersects(box)) {
                            result.push_back(object);
                        }
                    }
                }
            }
        }
    }

    return result;
}

QList< GeoGraphicsItem* > GeoGraphicsScene::selectedItems() const
{
    return d->m_selectedItems;
}

void GeoGraphicsScene::applyHighlight( const QVector< GeoDataPlacemark* > &selectedPlacemarks )
{
    /**
     * First set the items, which were selected previously, to
     * use normal style
     */
    foreach ( GeoGraphicsItem *item, d->m_selectedItems ) {
        item->setHighlighted( false );
    }

    // Also clear the list to store the new selected items
    d->m_selectedItems.clear();

    /**
     * Process the placemark. which were under mouse
     * while clicking, and update corresponding graphics
     * items to use highlight style
     */
    foreach( const GeoDataPlacemark *placemark, selectedPlacemarks ) {
        QList<TileId> tiles = d->m_features.values( placemark );
        foreach( const TileId &tileId, tiles ) {
            QList<GeoGraphicsItem*> clickedItems = d->m_items[tileId];
            foreach ( GeoGraphicsItem *item, clickedItems ) {
                if ( item->feature() == placemark ) {
                    GeoDataObject *parent = placemark->parent();
                    if ( parent ) {
                        if ( parent->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
                            GeoDataDocument *doc = static_cast<GeoDataDocument*>( parent );
                            QString styleUrl = placemark->styleUrl();
                            styleUrl.remove(QLatin1Char('#'));
                            if ( !styleUrl.isEmpty() ) {
                                GeoDataStyleMap const &styleMap = doc->styleMap( styleUrl );
                                GeoDataStyle::Ptr style = d->highlightStyle( doc, styleMap );
                                if ( style ) {
                                    d->selectItem( item );
                                    d->applyHighlightStyle( item, style );
                                }
                            }

                            /**
                            * If a placemark is using an inline style instead of a shared
                            * style ( e.g in case when theme file specifies the colorMap
                            * attribute ) then highlight it if any of the style maps have a
                            * highlight styleId
                            */
                            else {
                                foreach ( const GeoDataStyleMap &styleMap, doc->styleMaps() ) {
                                    GeoDataStyle::Ptr style = d->highlightStyle( doc, styleMap );
                                    if ( style ) {
                                        d->selectItem( item );
                                        d->applyHighlightStyle( item, style );
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    emit repaintNeeded();
}

void GeoGraphicsScene::removeItem( const GeoDataFeature* feature )
{
    QList<TileId> keys = d->m_features.values( feature );
    foreach( const TileId& key, keys ) {
        QList< GeoGraphicsItem* >& tileList = d->m_items[key];
        foreach( GeoGraphicsItem* item, tileList ) {
            if( item->feature() == feature ) {
                d->m_features.remove( feature );
                tileList.removeAll( item );
                delete item;
                break;
            }
        }
    }
}

void GeoGraphicsScene::clear()
{
    foreach(const QList<GeoGraphicsItem*> &list, d->m_items.values()) {
        qDeleteAll(list);
    }
    d->m_items.clear();
    d->m_features.clear();
}

void GeoGraphicsScene::addItem( GeoGraphicsItem* item )
{
    // Select zoom level so that the object fit in single tile
    int zoomLevel;
    qreal north, south, east, west;
    item->latLonAltBox().boundaries( north, south, east, west );
    for(zoomLevel = item->minZoomLevel(); zoomLevel >= 0; zoomLevel--)
    {
        if( TileId::fromCoordinates( GeoDataCoordinates(west, north, 0), zoomLevel ) ==
            TileId::fromCoordinates( GeoDataCoordinates(east, south, 0), zoomLevel ) )
            break;
    }

    const TileId key = TileId::fromCoordinates( GeoDataCoordinates(west, north, 0), zoomLevel ); // same as GeoDataCoordinates(east, south, 0), see above

    QList< GeoGraphicsItem* >& tileList = d->m_items[key];
    QList< GeoGraphicsItem* >::iterator position = qLowerBound( tileList.begin(), tileList.end(), item, GeoGraphicsItem::zValueLessThan );
    tileList.insert( position, item );
    d->m_features.insert( item->feature(), key );
}

}

#include "moc_GeoGraphicsScene.cpp"
