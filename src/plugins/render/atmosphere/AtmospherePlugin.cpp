//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "AtmospherePlugin.h"
#include "Planet.h"

#include "GeoPainter.h"
#include "ViewportParams.h"
#include "MarbleModel.h"

namespace Marble
{

AtmospherePlugin::AtmospherePlugin() :
    RenderPlugin( 0 )
{
}

AtmospherePlugin::AtmospherePlugin( const MarbleModel *marbleModel ) :
    RenderPlugin( marbleModel )
{
}

QStringList AtmospherePlugin::backendTypes() const
{
    return QStringList( "atmosphere" );
}

QString AtmospherePlugin::renderPolicy() const
{
    return QString( "SPECIFIED_ALWAYS" );
}

QStringList AtmospherePlugin::renderPosition() const
{
    return QStringList() << "SURFACE";
}

QString AtmospherePlugin::name() const
{
    return tr( "Atmosphere" );
}

QString AtmospherePlugin::guiString() const
{
    return tr( "&Atmosphere" );
}

QString AtmospherePlugin::nameId() const
{
    return "atmosphere";
}

QString AtmospherePlugin::version() const
{
    return "1.0";
}

QString AtmospherePlugin::description() const
{
    return tr( "Shows the atmosphere around the earth." );
}

QIcon AtmospherePlugin::icon() const
{
    return QIcon();
}

QString AtmospherePlugin::copyrightYears() const
{
    return "2006-2012";
}

QList<PluginAuthor> AtmospherePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Torsten Rahn", "tackat@kde.org" )
            << PluginAuthor( "Inge Wallin", "ingwa@kde.org" )
            << PluginAuthor( "Jens-Michael Hoffmann", "jmho@c-xx.com" )
            << PluginAuthor( "Patrick Spendrin", "ps_ml@gmx.de" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" )
            << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" );
}

qreal AtmospherePlugin::zValue() const
{
    return -100.0;
}

void AtmospherePlugin::initialize()
{
    /* nothing to do */
}

bool AtmospherePlugin::isInitialized() const
{
    return true;
}

bool AtmospherePlugin::render( GeoPainter *painter,
                              ViewportParams *viewParams,
                              const QString &renderPos,
                              GeoSceneLayer *layer )
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    if ( !visible()  || !marbleModel()->planet()->hasAtmosphere() )
        return true;

    // Only draw an atmosphere if projection is spherical
    if ( viewParams->projection() != Spherical )
        return true;

    // No use to draw atmosphere if it's not visible in the area.
    if ( viewParams->mapCoversViewport() )
        return true;

    // Ok, now we know that at least a little of the atmosphere is
    // visible, if nothing else in the corners.  Draw the atmosphere
    // by using a circular gradient.  This is a pure visual effect and
    // has nothing to do with real physics.

    int  imageHalfWidth  = viewParams->width() / 2;
    int  imageHalfHeight = viewParams->height() / 2;

    QColor color = marbleModel()->planet()->atmosphereColor();

    // Recalculate the atmosphere effect and paint it to canvasImage.
    QRadialGradient grad1( QPointF( imageHalfWidth, imageHalfHeight ),
                           1.05 * viewParams->radius() );
    grad1.setColorAt( 0.91, color );
    grad1.setColorAt( 1.00, QColor(color.red(), color.green(), color.blue(), 0) );

    QBrush    brush1( grad1 );
    QPen      pen1( Qt::NoPen );

    painter->setBrush( brush1 );
    painter->setPen( pen1 );
    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->drawEllipse( imageHalfWidth  - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                          imageHalfHeight - (int) ( (qreal) ( viewParams->radius() ) * 1.05 ),
                          (int) ( 2.1 * (qreal) ( viewParams->radius()) ),
                          (int) ( 2.1 * (qreal) ( viewParams->radius()) ) );

    return true;
}

}

Q_EXPORT_PLUGIN2( AtmospherePlugin, Marble::AtmospherePlugin )

#include "AtmospherePlugin.moc"
