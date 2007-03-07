#include "katlastoolbox.h"

#include <QtAlgorithms>
#include <QtGui/QStringListModel>
#include "maptheme.h"

#include <QtCore/QDebug>

KAtlasToolBox::KAtlasToolBox(QWidget *parent) : QWidget(parent) {

	setupUi(this);
	minimumzoom = 950;

	setFocusPolicy(Qt::NoFocus);

	toolBoxTab1->setBackgroundRole(QPalette::Window);
	toolBoxTab2->setBackgroundRole(QPalette::Window);

	connect(goHomeButton, SIGNAL(clicked()), this, SIGNAL(goHome())); 
	connect(zoomSlider, SIGNAL(valueChanged(int)), this, SIGNAL(zoomChanged(int))); 
	connect(zoomInButton, SIGNAL(clicked()), this, SIGNAL(zoomIn())); 
	connect(zoomOutButton, SIGNAL(clicked()), this, SIGNAL(zoomOut())); 

	connect(moveLeftButton, SIGNAL(clicked()), this, SIGNAL(moveLeft())); 
	connect(moveRightButton, SIGNAL(clicked()), this, SIGNAL(moveRight())); 
	connect(moveUpButton, SIGNAL(clicked()), this, SIGNAL(moveUp())); 
	connect(moveDownButton, SIGNAL(clicked()), this, SIGNAL(moveDown())); 

	connect(searchLineEdit, SIGNAL(textChanged(QString)), locationListView, SLOT(selectItem(QString)));
	connect(searchLineEdit, SIGNAL(textChanged(const QString&)), locationListView, SLOT(activate()));

	connect(locationListView, SIGNAL(centerOn(const QModelIndex&)), this, SIGNAL(centerOn(const QModelIndex&)));

	QStringList mapthemedirs = MapTheme::findMapThemes( "maps" );

	QStandardItemModel* mapthememodel = MapTheme::mapThemeModel( mapthemedirs );
	katlasThemeSelectView->setModel( mapthememodel );

	connect(katlasThemeSelectView, SIGNAL(selectMapTheme( const QString& )), this, SIGNAL(selectMapTheme( const QString& )));
}

void KAtlasToolBox::changeZoom(int zoom){
// No infinite loops here
//	if (zoomSlider->value() != zoom)
		zoomSlider->setValue(zoom);
		zoomSlider->setMinimum( minimumzoom );
}
