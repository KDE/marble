//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmAnnotatePlugin.h"

#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtGui/QPushButton>

//#include <Phonon/MediaObject>
//#include <Phonon/VideoWidget>

#include <QtCore/QDebug>
#include "ViewportParams.h"
#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "TextAnnotation.h"

namespace Marble
{

QStringList OsmAnnotatePlugin::backendTypes() const
{
    return QStringList( "osmannotation" );
}

QString OsmAnnotatePlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList OsmAnnotatePlugin::renderPosition() const
{
    return QStringList( "ALWAYS_ON_TOP" );
}

QString OsmAnnotatePlugin::name() const
{
    return tr( "OSM Annotation Plugin" );
}

QString OsmAnnotatePlugin::guiString() const
{
    return tr( "&OSM Annotation Plugin" );
}

QString OsmAnnotatePlugin::nameId() const
{
    return QString( "osm-annotation-plugin" );
}

QString OsmAnnotatePlugin::description() const
{
    return tr( "This is a render and interaciton plugin used for annotating OSM data." );
}

QIcon OsmAnnotatePlugin::icon () const
{
    return QIcon();
}


void OsmAnnotatePlugin::initialize ()
{
    //initialise the first test widget
    QPushButton * button;
    button = new QPushButton(0);
    but = button;

    //Setup the model
    GeoDataCoordinates madrid( -13.7, 40.4, 0.0, GeoDataCoordinates::Degree );
    TextAnnotation* annon = new TextAnnotation();

    annon->setCoordinate(madrid);
    
    //FIXME memory leak withouth a model to do memory managment
    model.append(annon);

    //Attempted to add a video widget as a tech preview but this needs to
    //be properly considered. Phonon includes in a plugin?
//    video = new Phonon::VideoWidget(0);
//
//    vid = video;
//
//    Phonon::VideoWidget * video;
//    Phonon::MediaObject m;
//    QString fileName("/home/foo/bar.ogg");
//
//    m.setCurrentSource(fileName);


}

bool OsmAnnotatePlugin::isInitialized () const
{
    return true;
}

bool OsmAnnotatePlugin::render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    //Set the parents if they have not already been set

//    if ( vid->parent() == 0 ) {
//        vid->setParent( (QWidget*)painter->device() );
//        //start video
//    }

    if ( but->parent() == 0 ) {
        but->setParent( (QWidget*)painter->device() );
         but->setVisible(false);
    }

    int x, y;
    bool hidden;

    GeoDataCoordinates madrid( -13.7, 40.4, 0.0, GeoDataCoordinates::Degree );
    
    QListIterator<TmpGraphicsItem*> i(model);
    
    while(i.hasNext()) {
        TmpGraphicsItem* tmp= i.next();
        tmp->paint(painter, viewport, renderPos, layer);
    }




    viewport->currentProjection()->screenCoordinates( madrid, viewport, x, y, hidden );

    if( !hidden ) {
        but->move(QPoint(x, y));
        but->setVisible(false);
    } else {
        but->setVisible(false);
    }

    return true;
}

bool    OsmAnnotatePlugin::eventFilter(QObject* watched, QEvent* event)
{
    //FIXME why is the QEvent::MousePress not working? caught somewhere else?
    //does this mean we need to centralise the event handeling?
    if ( event->type() == QEvent::MouseMove ) {
        //if the event is in an item change cursor
        // FIXME make this more effecient by using the bsptree
        QListIterator<TmpGraphicsItem*> i(model);
        while(i.hasNext()) {
            //FIXME this point is impossible because we don't have access to a view.
            //------------------------------------
            //you either need to cache the painted screen positions
            //or
            //invent a GeoEvent that contains geo points.
            //-------------------------------------
            i.next();
        }


        qDebug() << "Event in OsmAnnotate Plugin" << event;
    }
    return false;
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
