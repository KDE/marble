//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleCrosshairsPlugin.h"

#include <QtCore/QDebug>
#include "GeoPainter.h"
#include "MarbleDirs.h"

#include "ViewportParams.h"

namespace Marble
{

QStringList MarbleCrosshairsPlugin::backendTypes() const
{
    return QStringList( "crosshairs" );
}

QString MarbleCrosshairsPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList MarbleCrosshairsPlugin::renderPosition() const
{
    return QStringList( "FLOAT_ITEM" ); // although this is not a float item we choose the position of one
}

QString MarbleCrosshairsPlugin::name() const
{
    return tr( "Crosshairs Plugin" );
}

QString MarbleCrosshairsPlugin::guiString() const
{
    return tr( "Cross&hairs" );
}

QString MarbleCrosshairsPlugin::nameId() const
{
    return QString( "crosshairs" );
}

QString MarbleCrosshairsPlugin::description() const
{
    return tr( "A plugin that shows crosshairs." );
}

QIcon MarbleCrosshairsPlugin::icon () const
{
    return QIcon();
}

void MarbleCrosshairsPlugin::initialize ()
{
}

bool MarbleCrosshairsPlugin::isInitialized () const
{
    return true;
}

bool MarbleCrosshairsPlugin::render( GeoPainter *painter, ViewportParams *viewport,
				const QString& renderPos,
				GeoSceneLayer * layer )
{
    int  centerx  = viewport->width() / 2;
    int  centery  = viewport->height() / 2;
    int  halfsize = 5;

    painter->save();

    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen( QColor( Qt::white ) );
    painter->drawLine( centerx - halfsize, centery,
                        centerx + halfsize, centery );
    painter->drawLine( centerx, centery - halfsize,
                        centerx, centery + halfsize );
    painter->restore();

    return true;
}

}

Q_EXPORT_PLUGIN2(MarbleCrosshairsPlugin, Marble::MarbleCrosshairsPlugin)

#include "MarbleCrosshairsPlugin.moc"
