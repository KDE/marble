//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#include "GpxFileViewItem.h"

#ifdef QTONLY
    #include <QtGui/QFileDialog>
#else
    #include <KFileDialog>
    #include <KUrl>
#endif

#include "gps/GpxFile.h"

using namespace Marble;

GpxFileViewItem::GpxFileViewItem( GpxFile* file ) :
    m_gpxFile( file )
{
}

GpxFileViewItem::~GpxFileViewItem()
{
    delete m_gpxFile;
}

void GpxFileViewItem::saveFile()
{
    QString fileName;

#ifdef QTONLY
    fileName = QFileDialog::getSaveFileName( 0, tr( "Save File" ),
            QString(),
            tr( "GpxFile (*.gpx)" ) );
#else
    fileName = KFileDialog::getSaveFileName( KUrl(),
                                             tr( "GpxFile (*.gpx)" ),
                                             0,
                                             tr( "Save File" ) );
#endif

    QFile file( fileName );
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
        return;
    }

    QTextStream stream( &file );
    stream << ( *m_gpxFile );
}

void GpxFileViewItem::closeFile( int start, bool finalize )
{
    Q_UNUSED( start );
    Q_UNUSED( finalize );
    //TODO
}

bool GpxFileViewItem::isShown() const
{
    return (m_gpxFile->checkState() == Qt::Checked );
}

void GpxFileViewItem::setShown( bool value )
{
    m_gpxFile->setCheckState( value );
}

QString GpxFileViewItem::name() const
{
    return m_gpxFile->display();
}
