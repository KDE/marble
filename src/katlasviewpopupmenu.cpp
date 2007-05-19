#include "katlasviewpopupmenu.h"

#include <QtCore/QDebug>
#include <QtGui/QMenu>

#ifndef Q_OS_MACX
#include "katlasviewpopupmenu.moc"
#endif

#include "katlasaboutdialog.h"
#include "katlasview.h"
#include "katlasglobe.h"
#include "placemark.h"
#include "placemarkinfodialog.h"

KAtlasViewPopupMenu::KAtlasViewPopupMenu(KAtlasView *view, KAtlasGlobe *globe)
    : m_globe(globe),
      m_view(view) 
{
    m_lmbMenu = new QMenu( m_view );
    m_rmbMenu = new QMenu( m_view );

    connect( m_lmbMenu, SIGNAL( triggered( QAction* ) ),
             this,      SLOT( showFeatureInfo( QAction* ) ) );
    createActions();
}

void KAtlasViewPopupMenu::createActions()
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

    m_pAboutDialogAction = new QAction( tr( "&About" ), this );
    m_rmbMenu->addAction( m_pAboutDialogAction );

    connect( m_pAddMeasurePointAction, SIGNAL( triggered() ),
                                       SLOT( slotAddMeasurePoint() ) );
    connect( m_pRemoveMeasurePointsAction, SIGNAL( triggered() ),
                                           SIGNAL( removeMeasurePoints() ) );
    connect( m_pAboutDialogAction, SIGNAL( triggered() ), 
                                   SLOT( slotAboutDialog() ) );
}


void KAtlasViewPopupMenu::showLmbMenu( int xpos, int ypos )
{
    m_lmbMenu->clear();

    QPoint  curpos = QPoint( xpos, ypos ); 
    m_featurelist = m_globe-> whichFeatureAt( curpos );

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

    float  lat;
    float  lng;

    m_view->globeSphericals( xpos, ypos, lng, lat );

    // Any idea what this could do on activation?
    m_posaction->setEnabled( false );
    m_posaction->setText( GeoPoint( lng, lat ).toString() );
    m_lmbMenu->addAction( m_posaction );

    m_lmbMenu->popup( m_view->mapToGlobal( curpos ) );
}


void KAtlasViewPopupMenu::showRmbMenu( int xpos, int ypos )
{
    QPoint curpos = QPoint( xpos, ypos );
    m_pAddMeasurePointAction->setData( curpos );
    m_rmbMenu->popup( m_view->mapToGlobal( curpos ) );
}


void KAtlasViewPopupMenu::showFeatureInfo( QAction* action )
{
    int         actionidx = action->data().toInt();
    PlaceMark  *mark      = 0;

    if ( actionidx > 0 ) {
        mark = m_featurelist.at( actionidx -1 );

        PlaceMarkInfoDialog  dialog( mark, m_view );
        dialog.exec();
    }
}


void KAtlasViewPopupMenu::slotAddMeasurePoint()
{
    QPoint  p = m_pAddMeasurePointAction->data().toPoint();

    float   lat;
    float   lng;

    m_view->globeSphericals( p.x(), p.y(), lng, lat);

    emit addMeasurePoint( lng, lat );
}

void KAtlasViewPopupMenu::slotAboutDialog()
{
    KAtlasAboutDialog dlg( m_view );
    dlg.exec();
}


#ifndef Q_OS_MACX
#include "katlasaboutdialog.moc"
#endif
