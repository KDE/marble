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

#include "GeoDataAnimatedUpdate.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataCreate.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "GeoDataFolder.h"
#include "GeoDataGroundOverlay.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"

#include <QString>

namespace Marble
{
PlaybackAnimatedUpdateItem::PlaybackAnimatedUpdateItem( GeoDataAnimatedUpdate* animatedUpdate )
{
    m_animatedUpdate = animatedUpdate;
    m_rootDocument = rootDocument( m_animatedUpdate );
    m_playing = false;
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
    if( m_playing ){
        return;
    }
    m_playing = true;

    if ( !m_rootDocument || !m_animatedUpdate->update() ) {
        return;
    }

    // Apply updates of elements
    if ( m_animatedUpdate->update()->change() ) {
        QVector<GeoDataPlacemark*> placemarkList = m_animatedUpdate->update()->change()->placemarkList();
        for( int i = 0; i < placemarkList.size(); i++ ){
            GeoDataPlacemark* placemark = placemarkList.at( i );
            QString targetId = placemark->targetId();
            if( targetId.isEmpty() ) {
                continue;
            }
            if( placemark->isBalloonVisible() ){
                GeoDataFeature* feature = findFeature( m_rootDocument, targetId );
                if (auto placemark = geodata_cast<GeoDataPlacemark>(feature)) {
                    emit balloonShown(placemark);
                }
            } else {
                emit balloonHidden();
            }
        }
    }

    // Create new elements
    if( m_animatedUpdate->update()->create() ){
        for( int index = 0; index < m_animatedUpdate->update()->create()->size(); ++index ) {
            GeoDataFeature* child = m_animatedUpdate->update()->create()->child( index );
            if( child &&
                    (geodata_cast<GeoDataDocument>(child)||
                     geodata_cast<GeoDataFolder>(child))) {
                GeoDataContainer *addContainer = static_cast<GeoDataContainer*>( child );
                QString targetId = addContainer->targetId();
                GeoDataFeature* feature = findFeature( m_rootDocument, targetId );
                if( feature &&
                        (geodata_cast<GeoDataDocument>(feature) ||
                         geodata_cast<GeoDataFolder>(feature))) {
                    GeoDataContainer* container = static_cast<GeoDataContainer*>( feature );
                    for( int i = 0; i < addContainer->size(); ++i ) {
                        emit added( container, addContainer->child( i ), -1 );
                        if (auto placemark = geodata_cast<GeoDataPlacemark>(addContainer->child(i)))
                        {
                            if( placemark->isBalloonVisible() ) {
                                emit balloonShown( placemark );
                            }
                        }
                    }
                }

            }
        }
    }

    // Delete elements
    if( m_animatedUpdate->update()->getDelete() ){
        for( int index = 0; index < m_animatedUpdate->update()->getDelete()->size(); ++index ) {
            GeoDataFeature* child = m_animatedUpdate->update()->getDelete()->child( index );
            QString targetId = child->targetId();
            if( targetId.isEmpty() ) {
                continue;
            }
            GeoDataFeature* feature = findFeature( m_rootDocument, targetId );
            if (feature && canDelete(*feature)) {
                m_deletedObjects.append( feature );
                emit removed( feature );
                if (auto placemark = geodata_cast<GeoDataPlacemark>(feature))
                {
                    if( placemark->isBalloonVisible() ) {
                        emit balloonHidden();
                    }
                }
            }
        }
    }
}

GeoDataFeature* PlaybackAnimatedUpdateItem::findFeature(GeoDataFeature* feature, const QString& id ) const
{
    if ( feature && feature->id() == id ){
        return feature;
    }

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( feature );
    if ( container ){
        QVector<GeoDataFeature*>::Iterator end = container->end();
        QVector<GeoDataFeature*>::Iterator iter = container->begin();
        for( ; iter != end; ++iter ){
            GeoDataFeature *foundFeature = findFeature( *iter, id );
            if ( foundFeature ){
                return foundFeature;
            }
        }
    }
    return 0;
}

GeoDataDocument *PlaybackAnimatedUpdateItem::rootDocument( GeoDataObject* object ) const
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
    play();
}

void PlaybackAnimatedUpdateItem::stop()
{
    if( !m_playing ){
        return;
    }
    m_playing = false;

    if ( m_animatedUpdate->update()->change() ) {
        QVector<GeoDataPlacemark*> placemarkList = m_animatedUpdate->update()->change()->placemarkList();
        for( int i = 0; i < placemarkList.size(); i++ ){
            GeoDataPlacemark* placemark = placemarkList.at( i );
            QString targetId = placemark->targetId();
            if( targetId.isEmpty() ) {
                continue;
            }
            GeoDataFeature* feature = findFeature( m_rootDocument, targetId );
            if( placemark->isBalloonVisible() ){
                if (geodata_cast<GeoDataPlacemark>(feature)) {
                    emit balloonHidden();
                }
            } else {
                emit balloonShown( static_cast<GeoDataPlacemark*>( feature ) );
            }
        }
    }

    if( m_animatedUpdate->update()->create() ){
        for( int index = 0; index < m_animatedUpdate->update()->create()->size(); ++index ) {
            GeoDataFeature* feature = m_animatedUpdate->update()->create()->child( index );
            if( feature &&
                    (geodata_cast<GeoDataDocument>(feature) ||
                     geodata_cast<GeoDataFolder>(feature))) {
                GeoDataContainer* container = static_cast<GeoDataContainer*>( feature );
                for( int i = 0; i < container->size(); ++i ) {

                    emit removed( container->child( i ) );
                    if (auto placemark = geodata_cast<GeoDataPlacemark>(container->child(i)))
                    {
                        if( placemark->isBalloonVisible() ) {
                            emit balloonHidden();
                        }
                    }
                }
            }
        }
    }

    for( GeoDataFeature* feature: m_deletedObjects ) {
        if( feature->targetId().isEmpty() ) {
            continue;
        }
        GeoDataFeature* target = findFeature( m_rootDocument, feature->targetId() );
        if ( target ) {
            /** @todo Do we have to note the original row position and restore it? */
            Q_ASSERT( dynamic_cast<GeoDataContainer*>( target ) );
            emit added( static_cast<GeoDataContainer*>( target ), feature, -1 );
            if (auto placemark = geodata_cast<GeoDataPlacemark>(feature))
            {
                if( placemark->isBalloonVisible() ) {
                    emit balloonShown( placemark );
                }
            }
        } // else the root document was modified in an unfortunate way and we cannot restore it at this point
    }
    m_deletedObjects.clear();
}

bool PlaybackAnimatedUpdateItem::isApplied() const
{
    return m_playing;
}

bool PlaybackAnimatedUpdateItem::canDelete(const GeoDataFeature &feature)
{
    return  geodata_cast<GeoDataDocument>(&feature) ||
            geodata_cast<GeoDataFolder>(&feature) ||
            geodata_cast<GeoDataGroundOverlay>(&feature) ||
            geodata_cast<GeoDataPlacemark>(&feature) ||
            geodata_cast<GeoDataScreenOverlay>(&feature) ||
            geodata_cast<GeoDataPhotoOverlay>(&feature);
}

}

#include "moc_PlaybackAnimatedUpdateItem.cpp"
