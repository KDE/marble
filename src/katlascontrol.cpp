#include "katlascontrol.h"

#include <QLayout>
#include <QStringListModel>

#include "katlastoolbox.h"
#ifdef KDEBUILD
#include "katlastoolbox.moc"
#include "katlascontrol.moc"
#endif

KAtlasControl::KAtlasControl(QWidget *parent)
   : QWidget(parent)
{
	setWindowTitle(tr("Marble - Desktop Globe"));
//	resize(640, 640);
	resize(680, 640);

	toolbox = new KAtlasToolBox(this);
	toolbox -> setFixedWidth(185);

	gpview=new KAtlasView(this);
	gpview->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding));

	gpview->setMinimumZoom( toolbox->minimumZoom() );
//	QStringListModel* placemarks = new QStringListModel();

//	gpview->getPlaceMarks(placemarks);
	toolbox -> setLocations(gpview->getPlaceMarkModel());

//	toolbox -> setLocations(placemarks);

	QVBoxLayout *vlayout = new QVBoxLayout(this);

	QHBoxLayout *hlayout = new QHBoxLayout();	

	hlayout->addWidget(toolbox);
	hlayout->addWidget(gpview);
	vlayout->addLayout(hlayout);

	connect(toolbox, SIGNAL(goHome()), gpview, SLOT(goHome()));
	connect(toolbox, SIGNAL(zoomChanged(int)), gpview, SLOT(zoomView(int)));
	connect(toolbox, SIGNAL(zoomIn()), gpview, SLOT(zoomIn()));
	connect(toolbox, SIGNAL(zoomOut()), gpview, SLOT(zoomOut()));

	connect(toolbox, SIGNAL(moveLeft()), gpview, SLOT(moveLeft()));
	connect(toolbox, SIGNAL(moveRight()), gpview, SLOT(moveRight()));
	connect(toolbox, SIGNAL(moveUp()), gpview, SLOT(moveUp()));
	connect(toolbox, SIGNAL(moveDown()), gpview, SLOT(moveDown()));

	connect(gpview, SIGNAL(zoomChanged(int)), toolbox, SLOT(changeZoom(int)));
	connect(toolbox, SIGNAL(centerOn(const QModelIndex&)), gpview, SLOT(centerOn(const QModelIndex&)));
	connect(toolbox, SIGNAL(selectMapTheme(const QString&)), gpview, SLOT(setMapTheme(const QString&)));
}

void KAtlasControl::zoomIn()
{
	gpview->zoomIn();
}

void KAtlasControl::zoomOut()
{
	gpview->zoomOut();
}

void KAtlasControl::moveLeft()
{
	gpview->moveLeft();
}

void KAtlasControl::moveRight()
{
	gpview->moveRight();
}

void KAtlasControl::moveUp()
{
	gpview->moveUp();
}

void KAtlasControl::moveDown()
{
	gpview->moveDown();
}
