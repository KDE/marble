//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackAnimatedUpdateItem.h"
#include "GeoDataTypes.h"

#include <QString>

namespace Marble
{
PlaybackAnimatedUpdateItem::PlaybackAnimatedUpdateItem( const GeoDataAnimatedUpdate* animatedUpdate )
{
    m_animatedUpdate = animatedUpdate;
}

const GeoDataAnimatedUpdate* PlaybackAnimatedUpdateItem::animatedUpdate() const
{
    return m_animatedUpdate;
}

double PlaybackAnimatedUpdateItem::duration() const
{
    return m_animatedUpdate->duration();
}

void PlaybackAnimatedUpdateItem::play()
{
    if ( m_animatedUpdate->update() && m_animatedUpdate->update()->change() ) {
        QVector<GeoDataPlacemark*> placemarkList = m_animatedUpdate->update()->change()->placemarkList();
        for( int i = 0; i < placemarkList.size(); i++ ){
            GeoDataPlacemark* placemark = placemarkList.at( i );
            QString targetId = placemark->targetId();
            if( placemark->isBalloonVisible() ){
                GeoDataDocument* document = rootDocument( placemark );
                GeoDataPlacemark* placemarkFromDocument = findPlacemark( document, targetId );
                if( placemarkFromDocument ){
                    emit balloonShown( placemarkFromDocument );
                }
            } else {
                emit balloonHidden();
            }
        }
    }
}

GeoDataPlacemark* PlaybackAnimatedUpdateItem::findPlacemark( GeoDataFeature* feature, const QString& targetId ) const
{
    if ( feature && feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        if( static_cast<GeoDataPlacemark*>( feature )->id() == targetId ){
            return static_cast<GeoDataPlacemark*>( feature );
        }
    }

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( feature );
    if ( container ){
        QVector<GeoDataFeature*>::Iterator end = container->end();
        QVector<GeoDataFeature*>::Iterator iter = container->begin();
        for( ; iter != end; ++iter ){
            GeoDataPlacemark *placemark = findPlacemark( *iter, targetId );
            if ( placemark ) {
                return placemark;
            }
        }
    }
    return 0;
}

GeoDataDocument* PlaybackAnimatedUpdateItem::rootDocument( GeoDataObject* object ) const
{
    if( !object || !object->parent() ){
        GeoDataDocument* document = dynamic_cast<GeoDataDocument*>( object );
        return document;
    } else {
        return rootDocument( object->parent() );
    }
    return 0;
}

void PlaybackAnimatedUpdateItem::pause()
{
    //do nothing
}

void PlaybackAnimatedUpdateItem::seek( double position )
{
    Q_UNUSED( position );
    //do nothing
}

void PlaybackAnimatedUpdateItem::stop()
{
    //do nothing
}

}

#include "PlaybackAnimatedUpdateItem.moc"
