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

#include "OnfRunner.h"

#include "MarbleAbstractRunner.h"
#include "GeoOnfParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"

#include <QtCore/QString>
#include <QtCore/QBuffer>
#include <QtCore/QVector>
#include <QtCore/QUrl>

#include <QtNetwork/QHttp>

namespace Marble
{

/*
TODO: grade results individually instead of giving them all a score of 100
*/

OnfRunner::OnfRunner( QObject *parent ) : MarbleAbstractRunner( parent )
{
    m_http = new QHttp("gazetteer.openstreetmap.org");
    m_buffer = 0;
    
    connect( m_http, SIGNAL( requestFinished( int, bool ) ),
             this, SLOT( slotRequestFinished( int, bool ) ) );
}

OnfRunner::~OnfRunner()
{
    delete m_http;
    delete m_buffer;
}

GeoDataFeature::GeoDataVisualCategory OnfRunner::category() const
{
    return GeoDataFeature::OsmSite;
}

void OnfRunner::fail()
{
    //The manager needs to know when parsing starts and stops
    //in order to have a balanced count of active runners. So
    //we emit runnerFinished() to balance the previous failed runnerStarted()
    QVector<GeoDataPlacemark*> empty;
    emit runnerFinished( this, empty );
    return;
}

void OnfRunner::run()
{
    if( m_input.isEmpty() ) {
        fail();
    }
    //no point to keep downloading if we're doing a new one
    /* //causes crash
    if( m_http->currentId() != 0 ) {
        m_http->abort();
        //aborted parse request fails; this one keeps going.
        fail();
    }
    */
    //make a new buffer
    delete m_buffer;
    m_buffer = new QBuffer;
    QString base = "/namefinder/search.xml?find=%1";
    QString input = QUrl::toPercentEncoding(m_input);
    QString request = base.arg(input);
    mDebug() << "ONF search: GET " << request;
    m_http->get( request, m_buffer );
}

void OnfRunner::slotRequestFinished( int id, bool error )
{
    if( error ) {
        mDebug() << "ONF request" << id << "failed:" << m_http->error()
                 << m_http->errorString();
        fail();
        return;
    }
    
    mDebug() << "ONF search result buffer size:" << m_buffer->size();
    //mDebug() << m_buffer->data();
    
    QByteArray array = m_buffer->data();
    QBuffer data( &array );
    data.open( QIODevice::ReadOnly );
    
    GeoOnfParser parser;
    if( !parser.read( &data ) ) {
        qWarning() << "Could not parse ONF buffer!!";
        fail();
        return;
    }
    
    GeoDataDocument *results = static_cast<GeoDataDocument*>( parser.releaseDocument() );
    Q_ASSERT( results );

    QVector<GeoDataPlacemark*> placemarks = results->placemarkList();

    QVector<GeoDataPlacemark*>::iterator it = placemarks.begin();
    QVector<GeoDataPlacemark*>::iterator end = placemarks.end();
    for(; it != end; ++it ) {
        (*it)->setVisualCategory( category() );
    }
    emit runnerFinished( this, placemarks );
    delete results;
    return;
}

} // namespace Marble

#include "OnfRunner.moc"
