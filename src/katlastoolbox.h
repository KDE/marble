//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
// Copyright 2007      Thomas Zander <zander@kde.org>"
//

//
// The TextureColorizer maps the Elevationvalues to Legend Colors.
//


#ifndef KATLASTOOLBOX_H
#define KATLASTOOLBOX_H


#include "ui_katlastoolbox.h"

/**
@author Torsten Rahn
*/

class QStringListModel;

class KAtlasToolBox : public QWidget, private Ui::katlasToolBox
{
    Q_OBJECT

 public:
    KAtlasToolBox(QWidget *parent = 0);
 
   void setLocations(QAbstractItemModel* locations) {
       locationListView->setModel( locations );
   }
   int minimumZoom() const { return m_minimumzoom; }
	
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
    int  m_minimumzoom;
    int  m_maximumzoom;

 private:
    QString  m_searchTerm;
    bool     m_searchTriggered;
};

#endif // KATLASTOOLBOX_H
