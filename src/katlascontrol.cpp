#include "katlascontrol.h"

#include <QtGui/QLayout>
#include <QtGui/QStringListModel>

#include "katlastoolbox.h"


KAtlasControl::KAtlasControl(QWidget *parent)
   : QWidget(parent)
{
    setWindowTitle(tr("Marble - Desktop Globe"));
    //	resize(640, 640);
    resize( 680, 640 );

    m_toolbox = new KAtlasToolBox( this );
    m_toolbox->setFixedWidth( 185 );

    // Create the Model (Globe) and one view.
    m_globe = new KAtlasGlobe( this );
    m_katlasview = new KAtlasView( m_globe, this );
    m_katlasview->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
					      QSizePolicy::MinimumExpanding ) );
    m_katlasview->setMinimumZoom( m_toolbox->minimumZoom() );

    QVBoxLayout *vlayout = new QVBoxLayout( this );

    QHBoxLayout *hlayout = new QHBoxLayout();	

    hlayout->addWidget( m_toolbox );
    hlayout->addWidget( m_katlasview );
    vlayout->addLayout( hlayout );

    m_toolbox->setLocations( m_katlasview->placeMarkModel() );

    connect(m_toolbox, SIGNAL(goHome()),         m_katlasview, SLOT(goHome()));
    connect(m_toolbox, SIGNAL(zoomChanged(int)), m_katlasview, SLOT(zoomView(int)));
    connect(m_toolbox, SIGNAL(zoomIn()),  m_katlasview, SLOT(zoomIn()));
    connect(m_toolbox, SIGNAL(zoomOut()), m_katlasview, SLOT(zoomOut()));

    connect(m_toolbox, SIGNAL(moveLeft()),  m_katlasview, SLOT(moveLeft()));
    connect(m_toolbox, SIGNAL(moveRight()), m_katlasview, SLOT(moveRight()));
    connect(m_toolbox, SIGNAL(moveUp()),    m_katlasview, SLOT(moveUp()));
    connect(m_toolbox, SIGNAL(moveDown()),  m_katlasview, SLOT(moveDown()));

    connect(m_katlasview, SIGNAL(zoomChanged(int)), 
	    m_toolbox,    SLOT(changeZoom(int)));
    connect(m_toolbox,    SIGNAL(centerOn(const QModelIndex&)),
	    m_katlasview, SLOT(centerOn(const QModelIndex&)));
    connect(m_toolbox,    SIGNAL(selectMapTheme(const QString&)),
	    m_katlasview, SLOT(setMapTheme(const QString&)));
}


void KAtlasControl::zoomIn()
{
    m_katlasview->zoomIn();
}

void KAtlasControl::zoomOut()
{
    m_katlasview->zoomOut();
}

void KAtlasControl::moveLeft()
{
    m_katlasview->moveLeft();
}

void KAtlasControl::moveRight()
{
    m_katlasview->moveRight();
}

void KAtlasControl::moveUp()
{
    m_katlasview->moveUp();
}

void KAtlasControl::moveDown()
{
    m_katlasview->moveDown();
}


#ifndef Q_OS_MACX
#include "katlastoolbox.moc"
#include "katlascontrol.moc"
#endif
