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

#include "GeoTool.h"
#include "GeoShape.h"

#include <QToolButton>
#include <QGridLayout>
#include <KLocale>
#include <KIconLoader>
#include <KUrl>
#include <KFileDialog>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include <marble/MarbleControlBox.h>
#include <marble/MarbleMap.h>

GeoTool::GeoTool( KoCanvasBase* canvas )
    : KoTool( canvas ),
      m_geoshape(0)
{
}

void GeoTool::activate (bool temporary)
{
    Q_UNUSED( temporary );

    KoSelection* selection = m_canvas->shapeManager()->selection();
    foreach ( KoShape* shape, selection->selectedShapes() )
    {
        m_geoshape = dynamic_cast<GeoShape*>( shape );
        if ( m_geoshape )
            break;
    }
    if ( !m_geoshape )
    {
        emit done();
        return;
    }
    useCursor( Qt::ArrowCursor, true );
}

void GeoTool::deactivate()
{
  m_geoshape = 0;
}

QWidget * GeoTool::createOptionWidget()
{

    QWidget *optionWidget = new QWidget();
    QGridLayout *layout = new QGridLayout( optionWidget );

    MarbleControlBox *control = 0;

    control = new MarbleControlBox( optionWidget );
    layout->addWidget( control, 0, 0 );
    connect( control, SIGNAL( zoomIn()  ), this, SLOT( zoomIn() ) );
    connect( control, SIGNAL( zoomOut() ), this, SLOT( zoomOut() ) );
    connect( control, SIGNAL( moveLeft() ), this, SLOT( moveLeft() ) );
    connect( control, SIGNAL( moveRight() ), this, SLOT( moveRight() ) );
    connect( control, SIGNAL( moveUp() ), this, SLOT( moveUp() ) );
    connect( control, SIGNAL( moveDown() ), this, SLOT( moveDown() ) );
    
    connect( control, SIGNAL( projectionSelected( Projection ) ),
             this, SLOT( setProjection( Projection) ) );
             
    // TODO set the currently used projection
    //control->projectionSelected( m_geoshape->marbleMap()->projection() );
    
    return optionWidget;
}

void GeoTool::zoomIn() {
    
    kDebug()<<" GeoTool::slotZoomIn";
    if(m_geoshape) {
        m_geoshape->marbleMap()->zoomIn();
        m_geoshape->marbleMap()->setNeedsUpdate();
        m_geoshape->update();
    }
}

void GeoTool::zoomOut() {
    
    kDebug()<<" GeoTool::slotZoomOut";
    if(m_geoshape) {
        m_geoshape->marbleMap()->zoomOut();
        m_geoshape->update();
    }
}

void GeoTool::moveLeft() {
    
    kDebug()<<" GeoTool::slotMoveLeft";
    if(m_geoshape) {
        m_geoshape->marbleMap()->moveLeft();
        m_geoshape->update();
    }
}

void GeoTool::moveRight() {
    
    kDebug()<<" GeoTool::slotMoveRight";
    if(m_geoshape) {
        m_geoshape->marbleMap()->moveRight();
        m_geoshape->update();
    }
}

void GeoTool::moveUp() {
    
    kDebug()<<" GeoTool::slotMoveUp";
    if(m_geoshape) {
        m_geoshape->marbleMap()->moveUp();
        m_geoshape->update();
    }
}

void GeoTool::moveDown() {

    kDebug()<<" GeoTool::slotMoveDown";
    if(m_geoshape) {
        m_geoshape->marbleMap()->moveDown();
        m_geoshape->update();
    }
}

void GeoTool::setProjection(Projection projection) {
    kDebug()<<" GeoTool::slotSetProjection";
    if(m_geoshape) {
      m_geoshape->marbleMap()->setProjection(projection);
      m_geoshape->update();
    }
}

void GeoTool::mouseDoubleClickEvent( KoPointerEvent *event ) {
    if(m_canvas->shapeManager()->shapeAt(event->point) != m_geoshape) {
        event->ignore(); // allow the event to be used by another
        return;
    }

    //slotChangeUrl();
/*
    repaintSelection();
    updateSelectionHandler();
*/
}

#include "GeoTool.moc"
