//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
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
         but->setVisible(true);
    }

    int x, y;
    bool hidden;

    GeoDataCoordinates madrid( -3.7, 40.4, 0.0, GeoDataCoordinates::Degree );


    viewport->currentProjection()->screenCoordinates( madrid, viewport, x, y, hidden );

    if( !hidden ) {
        but->move(QPoint(x, y));
        but->setVisible(true);
    } else {
        but->setVisible(false);
    }

    return true;
}

}

Q_EXPORT_PLUGIN2( OsmAnnotatePlugin, Marble::OsmAnnotatePlugin )

#include "OsmAnnotatePlugin.moc"
