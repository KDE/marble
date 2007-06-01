//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// KAtlasThemeSelectView lets the user choose a map theme
//


#ifndef KATLASTHEMESELECTVIEW_H
#define KATLASTHEMESELECTVIEW_H


#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>
#include <QtGui/QListView>


class KAtlasThemeSelectView : public QListView
{
    Q_OBJECT

 public:
    KAtlasThemeSelectView(QWidget *parent = 0);
    // void setModel( QAbstractItemModel * model );

 protected:
    void resizeEvent(QResizeEvent* event);

 private slots:
    void selectedMapTheme( QModelIndex index );

 signals:
    void selectMapTheme( const QString& );
};

#endif // KATLASTHEMESELECTVIEW_H
