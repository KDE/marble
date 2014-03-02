//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "GeoDataPlaylist.h"
#include "GeoDataSoundCue.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataTourControl.h"
#include "GeoDataWait.h"
#include "GeoDataFlyTo.h"

#include "GeoDataTypes.h"

namespace Marble
{

bool GeoDataPlaylist::operator==(const GeoDataPlaylist& other) const
{
    if( this->m_primitives.size() != other.m_primitives.size() ){
        return false;
    }
    else{
        int index = 0;
        foreach( GeoDataTourPrimitive* m_primitive, m_primitives ){
            if( m_primitive->nodeType() != other.m_primitives.at( index )->nodeType() ){
                return false;
            }else{
                const char* node = m_primitive->nodeType();
                if ( node ==  GeoDataTypes::GeoDataAnimatedUpdateType ){
                    GeoDataAnimatedUpdate* update1 = static_cast<GeoDataAnimatedUpdate*>( m_primitive );
                    GeoDataAnimatedUpdate* update2 = static_cast<GeoDataAnimatedUpdate*>( other.m_primitives.at( index ) );
                    if( *update1 != *update2 ){
                        return false;
                    }
                }
                else if( node == GeoDataTypes::GeoDataSoundCueType ){
                    GeoDataSoundCue* cue1 = static_cast<GeoDataSoundCue*>( m_primitive );
                    GeoDataSoundCue* cue2 = static_cast<GeoDataSoundCue*>( other.m_primitives.at( index ) );
                    if ( *cue1 != *cue2 ){
                        return false;
                    }
                }
                else if( node == GeoDataTypes::GeoDataTourControlType ){
                    GeoDataTourControl* control1 = static_cast<GeoDataTourControl*>( m_primitive );
                    GeoDataTourControl* control2 = static_cast<GeoDataTourControl*>( other.m_primitives.at( index ) );
                    if( *control1 != *control2 ){
                        return false;
                    }
                }
                else if( node == GeoDataTypes::GeoDataWaitType ){
                    GeoDataWait* wait1 = static_cast<GeoDataWait*>( m_primitive );
                    GeoDataWait* wait2 = static_cast<GeoDataWait*>( other.m_primitives.at( index ) );
                    if( *wait1 != *wait2 ){
                        return false;
                    }
                }
                else if( node == GeoDataTypes::GeoDataFlyToType ){
                    GeoDataFlyTo* flyTo1 = static_cast<GeoDataFlyTo*>( m_primitive );
                    GeoDataFlyTo* flyTo2 = static_cast<GeoDataFlyTo*>( other.m_primitives.at( index ) );
                    if( *flyTo1 != *flyTo2 ){
                        return false;
                    }
                }
                index++;
            }
        }
        return true;
    }
}

bool GeoDataPlaylist::operator!=(const GeoDataPlaylist& other) const
{
    return !this->operator==(other);
}

const char *GeoDataPlaylist::nodeType() const
{
    return GeoDataTypes::GeoDataPlaylistType;
}

GeoDataTourPrimitive* GeoDataPlaylist::primitive(int id)
{
    if (size() <= id || id < 0) {
        return 0;
    }
    return m_primitives.at(id);
}

const GeoDataTourPrimitive* GeoDataPlaylist::primitive(int id) const
{
    if (size() <= id || id < 0) {
        return 0;
    }
    return m_primitives.at(id);
}

void GeoDataPlaylist::addPrimitive( GeoDataTourPrimitive *primitive )
{
    primitive->setParent( this );
    m_primitives.push_back( primitive );
}

void GeoDataPlaylist::insertPrimitive( int position, GeoDataTourPrimitive *primitive )
{
    primitive->setParent( this );
    int const index = qBound( 0, position, m_primitives.size() );
    m_primitives.insert( index, primitive );
}

void GeoDataPlaylist::removePrimitiveAt(int position)
{
    m_primitives.removeAt( position );
}

void GeoDataPlaylist::swapPrimitives( int positionA, int positionB )
{
    if ( qMin( positionA, positionB ) >= 0 && qMax( positionA, positionB ) < m_primitives.size() ) {
        m_primitives.swap( positionA, positionB );
    }
}

int GeoDataPlaylist::size() const
{
    return m_primitives.size();
}

} // namespace Marble
