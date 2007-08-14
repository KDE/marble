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
// MarbleThemeSelectView lets the user choose a map theme
//


#ifndef MARBLETHEMESELECTVIEW_H
#define MARBLETHEMESELECTVIEW_H


#include <QtCore/QDebug>
#include <QtGui/QStandardItemModel>
#include <QtGui/QListView>

#include "marble_export.h"

class MARBLE_EXPORT MarbleThemeSelectView : public QListView
{
    Q_OBJECT

 public:
    MarbleThemeSelectView(QWidget *parent = 0);
    // void setModel( QAbstractItemModel * model );

 protected:
    void resizeEvent(QResizeEvent* event);

 private Q_SLOTS:
    void selectedMapTheme( QModelIndex index );

 Q_SIGNALS:
    void selectMapTheme( const QString& );
};

#endif // KATLASTHEMESELECTVIEW_H
