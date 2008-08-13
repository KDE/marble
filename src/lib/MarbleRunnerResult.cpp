/*
    Copyright 2008 Henry de Valence <hdevalence@gmail.com>
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MarbleRunnerResult.h"

#include "PlaceMarkContainer.h"

MarbleRunnerResult::MarbleRunnerResult()
{
    PlaceMarkContainer empty;
    m_placemarks = empty;
    m_score = static_cast<Score>(0);
}

MarbleRunnerResult::MarbleRunnerResult( PlaceMarkContainer placemarks, Score score )
{
    m_placemarks = placemarks;
    m_score = score;
}

MarbleRunnerResult::MarbleRunnerResult( const MarbleRunnerResult &copied )
{
    m_placemarks = copied.placemarks();
    m_score = copied.score();
}

MarbleRunnerResult::Score MarbleRunnerResult::score() const
{
    return m_score;
}

bool MarbleRunnerResult::setScore( Score score )
{
    if( score >= 0 && score <= 100 ) {
        m_score = score;
        return true;
    } else {
        return false;
    }
}

PlaceMarkContainer MarbleRunnerResult::placemarks() const
{
    return m_placemarks;
}

void MarbleRunnerResult::setPlacemarks( PlaceMarkContainer placemarks )
{
    m_placemarks = placemarks;
}


