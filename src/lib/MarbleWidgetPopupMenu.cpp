//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleWidgetPopupMenu.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>

#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "placemark.h"
#include "placemarkinfodialog.h"

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget, 
                                         MarbleModel *model)
    : m_model(model),
      m_widget(widget) 
{
    m_lmbMenu = new QMenu( m_widget );
    m_rmbMenu = new QMenu( m_widget );

    connect( m_lmbMenu, SIGNAL( triggered( QAction* ) ),
             this,      SLOT( showFeatureInfo( QAction* ) ) );
    createActions();
}

void MarbleWidgetPopupMenu::createActions()
{
    //	Property actions (Left mouse button)
    //	m_earthaction = new QAction(QIcon("icon.png"), tr("&Earth"), this);
    m_earthaction = new QAction( tr( "&Earth" ), this );
    m_earthaction->setData( 0 );
    m_posaction = new QAction( tr( "0 N 0 W" ), this );
    m_posaction->setFont( QFont( "Sans Serif", 7, 50, false ) );

    //	Tool actions (Right mouse button)
    m_pAddMeasurePointAction = new QAction( tr( "&Add Measure Point" ), this);
    m_rmbMenu->addAction( m_pAddMeasurePointAction );
    m_pRemoveMeasurePointsAction = new QAction( tr( "&Remove Measure Points" ),
                                                this);
    m_rmbMenu->addAction( m_pRemoveMeasurePointsAction );
    m_rmbMenu->addSeparator();
    m_pSetHomePointAction  = new QAction( tr( "&Set Home Location" ), this);
    m_rmbMenu->addAction( m_pSetHomePointAction );
    m_rmbMenu->addSeparator();

    m_pAboutDialogAction = new QAction( tr( "&About" ), this );
    m_rmbMenu->addAction( m_pAboutDialogAction );

    connect( m_pSetHomePointAction,    SIGNAL( triggered() ),
                                       SLOT( slotSetHomePoint() ) );
    connect( m_pAddMeasurePointAction, SIGNAL( triggered() ),
                                       SLOT( slotAddMeasurePoint() ) );
    connect( m_pRemoveMeasurePointsAction, SIGNAL( triggered() ),
                                           SIGNAL( removeMeasurePoints() ) );
    connect( m_pAboutDialogAction, SIGNAL( triggered() ), 
                                   SLOT( slotAboutDialog() ) );
}


void MarbleWidgetPopupMenu::showLmbMenu( int xpos, int ypos )
{
    m_lmbMenu->clear();

    QPoint  curpos = QPoint( xpos, ypos ); 
    m_featurelist = m_model-> whichFeatureAt( curpos );

    int  actionidx = 1;

    QVector<PlaceMark*>::const_iterator  it;
    for ( it = m_featurelist.constBegin();
          it != m_featurelist.constEnd(); ++it ) 
    {
        QAction  *action = new QAction( (*it)->name(), m_lmbMenu );
        action->setData( actionidx );
        action->setIcon( (*it)->symbolPixmap() );
        m_lmbMenu->addAction( action );
        actionidx++;
    }

    // Not implemented yet ;-)
    m_earthaction->setEnabled( false );

    m_lmbMenu->addAction( m_earthaction );
    m_lmbMenu->addSeparator();

    double  lat;
    double  lon;

    m_widget->geoCoordinates( xpos, ypos, lon, lat, GeoPoint::Radian );

    // Any idea what this could do on activation?
    m_posaction->setEnabled( false );
    m_posaction->setText( GeoPoint( lon, lat ).toString() );
    m_lmbMenu->addAction( m_posaction );

    m_lmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showRmbMenu( int xpos, int ypos )
{
    QPoint curpos = QPoint( xpos, ypos );
    m_pSetHomePointAction->setData( curpos );
    m_pAddMeasurePointAction->setData( curpos );
    m_rmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showFeatureInfo( QAction* action )
{
    int         actionidx = action->data().toInt();
    PlaceMark  *mark      = 0;

    if ( actionidx > 0 ) {
        mark = m_featurelist.at( actionidx -1 );

        PlaceMarkInfoDialog  dialog( mark, m_widget );
        dialog.exec();
    }
}

void MarbleWidgetPopupMenu::slotSetHomePoint()
{
    QPoint  p = m_pSetHomePointAction->data().toPoint();

    double  lat;
    double  lon;

    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoPoint::Degree );
    if ( valid == true )
    {
//        qDebug() << "Setting Home Location: " << lon << ", " << lat;   
        m_widget->setHome( lon, lat, m_widget->zoom() );
    }
}

void MarbleWidgetPopupMenu::slotAddMeasurePoint()
{
    QPoint  p = m_pAddMeasurePointAction->data().toPoint();

    double  lat;
    double  lon;

    m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoPoint::Radian );

    emit addMeasurePoint( lon, lat );
}

void MarbleWidgetPopupMenu::slotAboutDialog()
{
    MarbleAboutDialog dlg( m_widget );
    dlg.exec();
}


#include "MarbleWidgetPopupMenu.moc"

