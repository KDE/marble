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

#include <QtGui/QClipboard>
#include <QtCore/QDebug>
#include <QtGui/QMenu>

#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "PlaceMarkInfoDialog.h"

using namespace Marble;

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget, 
                                         MarbleModel *model)
    : QObject(widget),
      m_model(model),
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
    m_copyCoordinateAction = new QAction( tr( "0 N 0 W" ), this );

    //	Tool actions (Right mouse button)
    m_addMeasurePointAction = new QAction( tr( "Add &Measure Point" ), this);
    m_rmbMenu->addAction( m_addMeasurePointAction );
    m_removeMeasurePointsAction = new QAction( tr( "&Remove Measure Points" ),
                                                this);
    m_removeMeasurePointsAction->setEnabled(false);
    m_rmbMenu->addAction( m_removeMeasurePointsAction );
    m_rmbMenu->addSeparator();
    m_setHomePointAction  = new QAction( tr( "&Set Home Location" ), this);
    m_rmbMenu->addAction( m_setHomePointAction );
    m_rmbMenu->addSeparator();

    m_aboutDialogAction = new QAction( tr( "&About" ), this );
    m_rmbMenu->addAction( m_aboutDialogAction );

    connect( m_setHomePointAction,    SIGNAL( triggered() ),
                                       SLOT( slotSetHomePoint() ) );
    connect( m_addMeasurePointAction, SIGNAL( triggered() ),
                                       SLOT( slotAddMeasurePoint() ) );
    connect( m_removeMeasurePointsAction, SIGNAL( triggered() ),
                                           SLOT( slotRemoveMeasurePoints() ) );
    connect( m_aboutDialogAction, SIGNAL( triggered() ), 
                                   SLOT( slotAboutDialog() ) );
    connect( m_copyCoordinateAction,SIGNAL( triggered() ),
                         SLOT( slotCopyCoordinates() ) );
}


void MarbleWidgetPopupMenu::showLmbMenu( int xpos, int ypos )
{
    m_lmbMenu->clear();

    QPoint  curpos = QPoint( xpos, ypos ); 
    m_featurelist = m_model->whichFeatureAt( curpos );

    int  actionidx = 1;

    QVector<QModelIndex>::const_iterator  it;
    for ( it = m_featurelist.constBegin();
          it != m_featurelist.constEnd(); ++it ) 
    {
        QAction  *action = new QAction( (*it).data().toString(), m_lmbMenu );
        action->setData( actionidx );
        action->setIcon( (*it).data( Qt::DecorationRole ).value<QPixmap>() );
        m_lmbMenu->addAction( action );
        actionidx++;
    }

    // Not implemented yet ;-)
    m_earthaction->setEnabled( false );

    m_lmbMenu->addAction( m_earthaction );
    m_lmbMenu->addSeparator();

    qreal  lat;
    qreal  lon;

    m_widget->geoCoordinates( xpos, ypos, lon, lat, GeoDataCoordinates::Radian );

    m_copyCoordinateAction->setEnabled( true );
    m_copyCoordinateAction->setText( tr("Copy Coordinates") );
    m_copyCoordinateAction->setData( curpos );

    QMenu *positionMenu = m_lmbMenu->addMenu( GeoDataCoordinates( lon, lat, GeoDataCoordinates::Radian ).toString() );
    positionMenu->menuAction()->setFont( QFont( "Sans Serif", 7, 50, false ) );
    positionMenu->addAction( m_copyCoordinateAction );

    m_lmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showRmbMenu( int xpos, int ypos )
{
    QPoint curpos = QPoint( xpos, ypos );
    m_setHomePointAction->setData( curpos );
    m_addMeasurePointAction->setData( curpos );
    m_rmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showFeatureInfo( QAction* action )
{
    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        QModelIndex index = m_featurelist.at( actionidx -1 );

        PlaceMarkInfoDialog dialog( index, m_widget );
        dialog.exec();
    }
}

void MarbleWidgetPopupMenu::slotSetHomePoint()
{
    QPoint  p = m_setHomePointAction->data().toPoint();

    qreal  lat;
    qreal  lon;

    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Degree );
    if ( valid == true )
    {
//        qDebug() << "Setting Home Location: " << lon << ", " << lat;   
        m_widget->setHome( lon, lat, m_widget->zoom() );
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    QPoint  p = m_copyCoordinateAction->data().toPoint();

    qreal  lon;
    qreal  lat;

    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
    if ( valid == true )
    {
        QString  positionString = GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Radian ).toString();
        QClipboard  *clipboard = QApplication::clipboard();

        clipboard->setText( positionString );
    }
}

void MarbleWidgetPopupMenu::slotAddMeasurePoint()
{
    QPoint  p = m_addMeasurePointAction->data().toPoint();

    qreal  lat;
    qreal  lon;

    m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );

    m_removeMeasurePointsAction->setEnabled(true);

    emit addMeasurePoint( lon, lat );
}

void MarbleWidgetPopupMenu::slotRemoveMeasurePoints()
{
    m_removeMeasurePointsAction->setEnabled(false);

    emit removeMeasurePoints();
}

void MarbleWidgetPopupMenu::slotAboutDialog()
{
    MarbleAboutDialog dlg( m_widget );
    dlg.exec();
}


#include "MarbleWidgetPopupMenu.moc"

