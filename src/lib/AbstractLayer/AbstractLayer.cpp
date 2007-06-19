//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>"
//


#include "AbstractLayer.h"

#include <QtCore/QSize>
#include <QtCore/QObject>

#include "Quaternion.h"


AbstractLayer::AbstractLayer( QObject *parent ) 
    : QObject( parent )
{
    //no layer common things have been so far identified
}

bool AbstractLayer::getPixelPosFromGeoPoint( double _lon, double _lat,
                                            QSize screenSize, 
                                            Quaternion invRotAxis, 
                                            int radius, int &xOut,
                                            int &yOut )
{
    Quaternion qpos(_lon,_lat); //temp
    qpos.rotateAroundAxis(invRotAxis);

    if ( qpos.v[Q_Z]>0 ) {
        xOut = (int)( ( screenSize.width()/2 ) 
                + ( radius * qpos.v[Q_X] ) );
        yOut = (int)( ( screenSize.height()/2 ) 
                + ( radius * qpos.v[Q_Y] ) );
        return true;
    } else {
        return false;
    }
}

bool AbstractLayer::getPixelPosFromGeoPoint( Quaternion position,
                                            QSize screenSize, 
                                            Quaternion invRotAxis,
                                            int radius, int &xOut, 
                                            int &yOut )
{
    Quaternion qpos = position; //temp
    qpos.rotateAroundAxis( invRotAxis );

    if( qpos.v[Q_Z]>0 ){
        xOut = (int)( ( screenSize.width()/2 )
                + ( radius * qpos.v[Q_X] ) );
        yOut = (int)( ( screenSize.height()/2 )
                + ( radius * qpos.v[Q_Y] ) );
        
        return true;
    } else {
        return false;
    }
}


#include "AbstractLayer.moc"
