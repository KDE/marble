//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
// Copyright 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_ROUTINGPROFILESWIDGET_H
#define MARBLE_ROUTINGPROFILESWIDGET_H

#include <QWidget>

#include "marble_export.h"

namespace Marble
{

class MarbleModel;

class MARBLE_EXPORT RoutingProfilesWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit RoutingProfilesWidget( MarbleModel *marbleModel );
    ~RoutingProfilesWidget();

    Q_PRIVATE_SLOT( d, void add() )
    Q_PRIVATE_SLOT( d, void configure() )
    Q_PRIVATE_SLOT( d, void remove() )
    Q_PRIVATE_SLOT( d, void moveUp() )
    Q_PRIVATE_SLOT( d, void moveDown() )
    Q_PRIVATE_SLOT( d, void updateButtons() )

 private:
    class Private;
    Private *const d;
};

}

#endif
