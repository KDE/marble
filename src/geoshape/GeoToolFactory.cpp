/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>
   Copyright 2008 Simon Schmeisser <mail_to_wrt@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <klocale.h>

#include "GeoShape.h"
#include "GeoTool.h"

#include "GeoToolFactory.h"


GeoToolFactory::GeoToolFactory( QObject* parent)
    : KoToolFactory( parent, "GeoToolFactoryId", i18n( "Geo Tool" ) )
{
    setToolTip( i18n( "map editing tool" ) );
    setIcon( "Geoshape" );
    setToolType( dynamicToolType() );
    setPriority( 1 );
    setActivationShapeId( GEOSHAPEID );
}

GeoToolFactory::~GeoToolFactory()
{
}

KoTool* GeoToolFactory::createTool( KoCanvasBase* canvas )
{
    return new GeoTool( canvas );
}

#include "GeoToolFactory.moc"


