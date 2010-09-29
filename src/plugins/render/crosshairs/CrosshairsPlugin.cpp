//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "CrosshairsPlugin.h"

#include "AbstractProjection.h"
#include "MarbleDebug.h"
#include "GeoPainter.h"

#include "ViewportParams.h"

namespace Marble
{

QStringList CrosshairsPlugin::backendTypes() const
{
    return QStringList( "crosshairs" );
}

QString CrosshairsPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList CrosshairsPlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" ); // although this is not a float item we choose the position of one
}

QString CrosshairsPlugin::name() const
{
    return tr( "Crosshairs" );
}

QString CrosshairsPlugin::guiString() const
{
    return tr( "Cross&hairs" );
}

QString CrosshairsPlugin::nameId() const
{
    return QString( "crosshairs" );
}

QString CrosshairsPlugin::description() const
{
    return tr( "A plugin that shows crosshairs." );
}

QIcon CrosshairsPlugin::icon () const
{
    return QIcon( ":/icons/crosshairs.png" );
}

void CrosshairsPlugin::initialize ()
{
}

bool CrosshairsPlugin::isInitialized () const
{
    return true;
}

bool CrosshairsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
                               const QString& renderPos,
                               GeoSceneLayer * layer )
{
    Q_UNUSED( layer )

    if ( renderPos == "ALWAYS_ON_TOP" ) {
        qreal  centerx  = viewport->width() / 2;
        qreal  centery  = viewport->height() / 2;
        int  boxwidth = 6;
        int  boxheight = 2;
        int  boxoffset = 4;

        GeoDataCoordinates focusPoint = viewport->focusPoint();
        if (!viewport->focusPointIsCenter()) {
            viewport->currentProjection()->screenCoordinates(focusPoint, viewport, centerx, centery);
        }

        painter->save();

        painter->setRenderHint( QPainter::Antialiasing, false );
        painter->setPen( QColor( Qt::black ) );
        painter->setBrush( QColor( Qt::white ) );
        painter->drawRect( centerx - boxoffset - boxwidth, centery - 1, boxwidth, boxheight );
        painter->drawRect( centerx + boxoffset, centery - 1, boxwidth, boxheight );

        painter->drawRect( centerx - 1, centery - boxoffset - boxwidth, boxheight, boxwidth );
        painter->drawRect( centerx - 1, centery + boxoffset, boxheight, boxwidth );

    /*
        painter->drawLine( centerx - halfsize, centery,
                            centerx + halfsize, centery );
        painter->drawLine( centerx, centery - halfsize,
                            centerx, centery + halfsize );
    */
        painter->restore();
    }
    return true;
}

}

Q_EXPORT_PLUGIN2( CrosshairsPlugin, Marble::CrosshairsPlugin )

#include "CrosshairsPlugin.moc"
