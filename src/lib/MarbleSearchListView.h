//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_MARBLESEARCHLISTVIEW_H
#define MARBLE_MARBLESEARCHLISTVIEW_H


#include <QtGui/QListView>

#include "marble_export.h"

class QString;

namespace Marble
{

class MARBLE_EXPORT MarbleSearchListView : public QListView
{
    Q_OBJECT

 public:
    explicit MarbleSearchListView(QWidget*);

 public Q_SLOTS:
    void  selectItem(const QString&);
    void  activate();

 private:
    Q_DISABLE_COPY( MarbleSearchListView )
    class Private;
    Private  * const d;
};

}

#endif
