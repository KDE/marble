//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

// Self
#include "MarbleWidgetPopupMenu.h"

// Marble
#include "AbstractDataPluginItem.h"
#include "MarbleAboutDialog.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "GeoDataPlacemark.h"
#include "PlacemarkInfoDialog.h"
#include "MarbleDebug.h"
#include "Planet.h"

// Qt
#include <QtCore/QMimeData>
#include <QtCore/QPointer>
#include <QtGui/QAction>
#include <QtGui/QClipboard>
#include <QtGui/QMenu>

using namespace Marble;

MarbleWidgetPopupMenu::MarbleWidgetPopupMenu(MarbleWidget *widget, 
                                         MarbleModel *model)
    : QObject(widget),
      m_model(model),
      m_widget(widget),
      m_lmbMenu( new QMenu( m_widget ) ),
      m_rmbMenu( new QMenu( m_widget ) )
{
    connect( m_lmbMenu, SIGNAL( triggered( QAction* ) ),
             this,      SLOT( showFeatureInfo( QAction* ) ) );
    createActions();
}

void MarbleWidgetPopupMenu::createActions()
{
    //	Property actions (Left mouse button)
    //	m_planetAction = new QAction(QIcon("icon.png"), tr("&Earth"), this);
    m_planetAction = new QAction( tr( "&Earth" ), this );
    m_planetAction->setData( 0 );
    m_copyCoordinateAction = new QAction( tr( "0 N 0 W" ), this );

    //	Tool actions (Right mouse button)
    m_rmbExtensionPoint = m_rmbMenu->addSeparator();
    m_setHomePointAction  = new QAction( tr( "&Set Home Location" ), this);
    m_rmbMenu->addAction( m_setHomePointAction );
    m_rmbMenu->addSeparator();
    
    QAction *reloadAction  = new QAction( tr( "Rel&oad Map" ), this);
    m_rmbMenu->addAction( reloadAction );
    m_rmbMenu->addSeparator();    

    m_aboutDialogAction = new QAction( tr( "&About" ), this );
    m_rmbMenu->addAction( m_aboutDialogAction );

    connect( m_setHomePointAction,    SIGNAL( triggered() ),
                                       SLOT( slotSetHomePoint() ) );
    connect( m_aboutDialogAction, SIGNAL( triggered() ), 
                                   SLOT( slotAboutDialog() ) );
    connect( m_copyCoordinateAction,SIGNAL( triggered() ),
                         SLOT( slotCopyCoordinates() ) );
    connect( reloadAction, SIGNAL(triggered()), m_widget, SLOT(reloadMap()));
}


void MarbleWidgetPopupMenu::showLmbMenu( int xpos, int ypos )
{
    m_lmbMenu->clear();

    QPoint  curpos = QPoint( xpos, ypos ); 
    m_featurelist = m_model->whichFeatureAt( curpos );

    int  actionidx = 1;

    QVector<QModelIndex>::const_iterator it = m_featurelist.constBegin();
    QVector<QModelIndex>::const_iterator const itEnd = m_featurelist.constEnd();
    for (; it != itEnd; ++it )
    {
        QAction *action = new QAction( (*it).data().toString(), m_lmbMenu );
        action->setData( actionidx );
        action->setIcon( (*it).data( Qt::DecorationRole ).value<QPixmap>() );
        m_lmbMenu->addAction( action );
        actionidx++;
    }
    
    m_itemList = m_model->whichItemAt( curpos );
    QList<AbstractDataPluginItem *>::const_iterator itW = m_itemList.constBegin();
    QList<AbstractDataPluginItem *>::const_iterator const itWEnd = m_itemList.constEnd();
    for (; itW != itWEnd; ++itW )
    {
        m_lmbMenu->addAction( (*itW)->action() );
    }

    // Not implemented yet ;-)
    m_planetAction->setEnabled( false );

    m_lmbMenu->addAction( m_planetAction );
    m_lmbMenu->addSeparator();

    // Setting the proper planet name
    QString targetString = m_model->planet()->name();
    mDebug() << "targetString" << targetString;
    // FIXME: this should be removed later on ...
    if ( targetString == "Earth" ) 
        targetString = tr( "&Earth" );

    m_planetAction->setText( targetString );

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
    m_rmbMenu->popup( m_widget->mapToGlobal( curpos ) );
}


void MarbleWidgetPopupMenu::showFeatureInfo( QAction* action )
{
    int actionidx = action->data().toInt();

    if ( actionidx > 0 ) {
        QModelIndex index = m_featurelist.at( actionidx -1 );

        QPointer<PlacemarkInfoDialog> dialog = new PlacemarkInfoDialog( index, m_widget );
        dialog->setWindowModality( Qt::WindowModal );
        dialog->exec();
        delete dialog;
    }
}

void MarbleWidgetPopupMenu::slotSetHomePoint()
{
    QPoint  p = m_setHomePointAction->data().toPoint();

    qreal  lat;
    qreal  lon;

    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Degree );
    if ( valid )
    {
//        mDebug() << "Setting Home Location: " << lon << ", " << lat;   
        m_widget->setHome( lon, lat, m_widget->zoom() );
    }
}

void MarbleWidgetPopupMenu::slotCopyCoordinates()
{
    QPoint  p = m_copyCoordinateAction->data().toPoint();

    qreal  lon;
    qreal  lat;

    bool valid = m_widget->geoCoordinates( p.x(), p.y(), lon, lat, GeoDataCoordinates::Radian );
    if ( valid )
    {
	const GeoDataCoordinates coordinates = GeoDataCoordinates( lon, lat, 0.0, GeoDataCoordinates::Radian );
	const qreal longitude_degrees = coordinates.longitude(GeoDataCoordinates::Degree);
	const qreal latitude_degrees = coordinates.latitude(GeoDataCoordinates::Degree);

	// importing this representation into Marble does not show anything,
	// but Merkaartor shows the point
	const QString kmlRepresentation = QString::fromLatin1(
	  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	  "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
	  "<Document>\n"
	  " <Placemark>\n"
// 	  "   <name></name>\n"
	  "   <Point>\n"
	  "     <coordinates>%1,%2</coordinates>\n"
	  "   </Point>\n"
	  " </Placemark>\n"
	  "</Document>\n"
	  "</kml>\n"
	  ).arg(longitude_degrees, 0, 'f', 10).arg(latitude_degrees, 0, 'f', 10);

	  // importing this data into Marble and Merkaartor works
	  const QString gpxRepresentation = QString::fromLatin1(
	    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
	    "<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" creator=\"trippy\" version=\"0.1\"\n"
	    " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
	    " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">\n"
	    "  <wpt lat=\"%1\" lon=\"%2\">\n"
// 	    "   <ele>%3</ele>\n"
//      "   <time></time>\n"
// 	    "   <name>%4</name>\n"
	    "  </wpt>\n"
	    "</gpx>\n"
	    ).arg(latitude_degrees, 0, 'f', 10).arg(longitude_degrees, 0, 'f', 10);

	    QString  positionString = coordinates.toString();

	    QMimeData * const myMimeData = new QMimeData();
	    myMimeData->setText(positionString);
	    myMimeData->setData(QLatin1String("application/vnd.google-earth.kml+xml"), kmlRepresentation.toUtf8());
	    myMimeData->setData(QLatin1String("application/gpx+xml"), gpxRepresentation.toUtf8());

	    QClipboard * const clipboard = QApplication::clipboard();
	    clipboard->setMimeData(myMimeData);
    }
}

void MarbleWidgetPopupMenu::slotAboutDialog()
{
    MarbleAboutDialog dlg( m_widget );
    dlg.exec();
}

void MarbleWidgetPopupMenu::addAction( Qt::MouseButton button, QAction* action )
{
    if ( button == Qt::RightButton ) {
        m_rmbMenu->insertAction( m_rmbExtensionPoint, action );
    }
}

#include "MarbleWidgetPopupMenu.moc"

