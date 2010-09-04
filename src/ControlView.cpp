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


#include "ControlView.h"

#include <QtGui/QLayout>
#include <QtGui/QSplitter>
#include <QtGui/QStringListModel>
#include <QtGui/QPrintDialog>
#include <QtGui/QPrintPreviewDialog>
#include <QtGui/QPrinter>
#include <QtGui/QPainter>
#include <QtCore/QPointer>

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MapThemeManager.h"

namespace Marble
{

ControlView::ControlView( QWidget *parent )
   : QWidget( parent )
{
    setWindowTitle( tr( "Marble - Desktop Globe" ) );

    resize( 680, 640 );

    QVBoxLayout *vlayout = new QVBoxLayout( this );
    vlayout->setMargin( 0 );

    m_splitter = new QSplitter( this );
    vlayout->addWidget( m_splitter );

    m_control = new MarbleControlBox( this );
    m_splitter->addWidget( m_control );
    m_splitter->setStretchFactor( m_splitter->indexOf( m_control ), 0 );

    m_marbleWidget = new MarbleWidget( this );
    m_marbleWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                                QSizePolicy::MinimumExpanding ) );

    m_splitter->addWidget( m_marbleWidget );
    m_splitter->setStretchFactor( m_splitter->indexOf( m_marbleWidget ), 1 );
    m_splitter->setSizes( QList<int>() << 180 << width() - 180 );

    m_control->addMarbleWidget( m_marbleWidget );

    // TODO: Creating a second MapThemeManager may not be the best solution here.
    // MarbleModel also holds one with a QFileSystemWatcher.
    m_mapThemeManager = new MapThemeManager;

    m_control->setMapThemeModel( m_mapThemeManager->mapThemeModel() );
    m_control->updateMapThemeView();
}

ControlView::~ControlView()
{
    delete m_mapThemeManager;
}

void ControlView::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void ControlView::zoomOut()
{
    m_marbleWidget->zoomOut();
}

void ControlView::moveLeft()
{
    m_marbleWidget->moveLeft();
}

void ControlView::moveRight()
{
    m_marbleWidget->moveRight();
}

void ControlView::moveUp()
{
    m_marbleWidget->moveUp();
}

void ControlView::moveDown()
{
    m_marbleWidget->moveDown();
}

void ControlView::setSideBarShown( bool show )
{
    m_control->setVisible( show );
}

void ControlView::setNavigationTabShown( bool show )
{
    m_control->setNavigationTabShown( show );
}

void ControlView::setLegendTabShown( bool show )
{
    m_control->setLegendTabShown( show );
}

void ControlView::setMapViewTabShown( bool show )
{
    m_control->setMapViewTabShown( show );
}

void ControlView::setCurrentLocationTabShown( bool show )
{
    m_control->setCurrentLocationTabShown( show );
}

void ControlView::setFileViewTabShown( bool show )
{
    m_control->setFileViewTabShown( show );
}

QString ControlView::defaultMapThemeId() const
{
    QStringList fallBackThemes;
    fallBackThemes << "earth/srtm/srtm.dgml";
    fallBackThemes << "earth/bluemarble/bluemarble.dgml";
    fallBackThemes << "earth/openstreetmap/openstreetmap.dgml";

    QStringList installedThemes;
    QList<GeoSceneDocument const*> themes = m_mapThemeManager->mapThemes();
    foreach(GeoSceneDocument const* theme, themes) {
        installedThemes << theme->head()->mapThemeId();
    }

    foreach(const QString &fallback, fallBackThemes) {
        if (installedThemes.contains(fallback)) {
            return fallback;
        }
    }

    if (installedThemes.size()) {
        return installedThemes.first();
    }

    return QString();
}

void ControlView::printMapScreenShot( QPointer<QPrintDialog> printDialog)
{
#ifndef QT_NO_PRINTER
    if (printDialog->exec() == QDialog::Accepted) {
        QPixmap mapPixmap = mapScreenShot();
        printPixmap( printDialog->printer(), mapPixmap );
    }
#endif
}

void ControlView::printPixmap( QPrinter * printer, const QPixmap& pixmap  )
{
#ifndef QT_NO_PRINTER
    QSize printSize = pixmap.size();
    QRect mapPageRect = printer->pageRect();
    printSize.scale( printer->pageRect().size(), Qt::KeepAspectRatio );
    QPoint printTopLeft( ( mapPageRect.width() - printSize.width() ) / 2 ,
                         ( mapPageRect.height() - printSize.height() ) / 2 );
    QRect mapPrintRect( printTopLeft, printSize );

    QPainter painter;
    if (!painter.begin(printer))
        return;
    painter.drawPixmap( mapPrintRect, pixmap, pixmap.rect() );
    painter.end();
#endif
}

// QPointer is used because of issues described in http://www.kdedevelopers.org/node/3919
void ControlView::printPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );

    QPointer<QPrintPreviewDialog> preview = new QPrintPreviewDialog( &printer, this );
    preview->setWindowFlags ( Qt::Window );
    connect( preview, SIGNAL( paintRequested( QPrinter * ) ), SLOT( paintPrintPreview( QPrinter * ) ) );
    preview->exec();
    delete preview;
#endif
}

void ControlView::paintPrintPreview( QPrinter * printer )
{
#ifndef QT_NO_PRINTER
    QPixmap mapPixmap = mapScreenShot();
    printPixmap( printer, mapPixmap );
#endif
}

}

#include "ControlView.moc"
