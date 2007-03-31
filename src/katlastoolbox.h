//
// C++ Interface: texcolorizer
//
// Description: TextureColorizer 

// The TextureColorizer maps the Elevationvalues to Legend Colors.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
// Copyright (C) 2007 Thomas Zander <zander@kde.org>
//
// Copyright: See COPYING file that comes with this distribution

#ifndef KATLASTOOLBOX_H
#define KATLASTOOLBOX_H

#include "ui_katlastoolbox.h"

/**
@author Torsten Rahn
*/

class QStringListModel;

class KAtlasToolBox : public QWidget, private Ui::katlasToolBox {

Q_OBJECT

public:
	KAtlasToolBox(QWidget *parent = 0);
	void setLocations(QAbstractItemModel* locations){ locationListView->setModel( locations ); }
	int minimumZoom() const { return minimumzoom; }
	
signals:
	void goHome();
	void zoomIn();
	void zoomOut();
	void zoomChanged(int);

	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void centerOn(const QModelIndex&);

	void selectMapTheme( const QString& );
public slots:
	void changeZoom(int);

private slots:
    /// called whenever the user types something new in the search box
    void searchLineChanged(const QString &search);
    /// called by the singleShot to initiate a search based on the searchLine
    void search();

protected:
	void resizeEvent ( QResizeEvent * );
	int minimumzoom;

private:
    QString m_searchTerm;
    bool m_searchTriggered;
};

#endif // KATLASTOOLBOX_H
