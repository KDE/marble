//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "AbstractLayerContainer.h"

#include "AbstractLayerData.h"
#include "ClipPainter.h"
#include "MarbleDebug.h"

#include <QtCore/QBitArray>
#include <QtCore/QPointF>

using namespace Marble;

AbstractLayerContainer::AbstractLayerContainer( int size )
{
    m_data = new QVector<AbstractLayerData*>( size );
    m_visible = new QBitArray( size );
    m_name = 0;
}

AbstractLayerContainer::AbstractLayerContainer(const QString &name,
                                               int size )
{
    m_data = new QVector<AbstractLayerData*>( size );
    m_visible = new QBitArray( size );

    m_name = new QString ( name );
}

AbstractLayerContainer::~AbstractLayerContainer()
{
    delete m_visible;
    delete m_data;
    delete m_name;
}

void AbstractLayerContainer::draw ( ClipPainter *painter, 
                                    const QPoint &point )
{
    // This does not apply to An AbstractLayerContainer.
    Q_UNUSED( painter );
    Q_UNUSED( point );

    //mDebug() << "AbstractLayerContainter::draw( ClipPainter, QPoint)";
}

void AbstractLayerContainer::draw(ClipPainter *painter, 
                                  const QSize &canvasSize, 
                                  ViewParams *viewParams )
{
    // FIXME: Readd this:
    /*
    if ( box.isValid() ) {
        if ( m_boundingBox->intersects( box ) ) {
            draw( painter, canvasSize, viewParams );
        }
    } else { 
        draw( painter, canvasSize, viewParams );
    }
    */

    const_iterator it;

    for( it = constBegin() ; it < constEnd() ; ++it ) {
        (*it)->draw( painter, canvasSize, viewParams );
    }
}

QString AbstractLayerContainer::name() const
{
    return *m_name; 
}

void AbstractLayerContainer::processVisible()
{
    QVector<AbstractLayerData*>::const_iterator i = m_data->constBegin();
    int temp;

    for ( ; i < m_data->constEnd() ; ++i ) {
        if ( (*i)->visible() ) {
            //iterator safety
            temp = m_data->indexOf ( *i );
            m_visible->setBit ( temp, true );
        }
    }
}

void AbstractLayerContainer::printToStream( QTextStream & out) const
{
    const_iterator it;

    for( it = constBegin(); it < constEnd(); ++it )
    {
        out << *(*it);
    }
}

qreal AbstractLayerContainer::distance ( const QPoint &a, 
                                          const QPoint &b )
{
    return (  ( ( a.x() - b.x() ) * ( a.x() - b.x() ) )
            + ( ( a.y() - b.y() ) * ( a.y() - b.y() ) ) );
}

qreal AbstractLayerContainer::distance ( const QPointF &a, 
                                          const QPointF &b )
{
    return (  ( ( a.x() - b.x() ) * ( a.x() - b.x() ) )
            + ( ( a.y() - b.y() ) * ( a.y() - b.y() ) ) );
}

void AbstractLayerContainer::manageMemory()
{
    for ( int i = 0 ; i < m_visible->size () ; ++i ) {
        if ( m_visible->testBit( i ) ) {
            if ( ! ( this->contains ( m_data->at( i ) ) ) ) {
                this->append( m_data->at( i ) );
            }
        }
    }
}
