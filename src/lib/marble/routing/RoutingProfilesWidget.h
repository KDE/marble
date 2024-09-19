// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Niko Sams <niko.sams@gmail.com>
// SPDX-FileCopyrightText: 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
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
    explicit RoutingProfilesWidget(MarbleModel *marbleModel);
    ~RoutingProfilesWidget() override;

    Q_PRIVATE_SLOT(d, void add())
    Q_PRIVATE_SLOT(d, void configure())
    Q_PRIVATE_SLOT(d, void remove())
    Q_PRIVATE_SLOT(d, void moveUp())
    Q_PRIVATE_SLOT(d, void moveDown())
    Q_PRIVATE_SLOT(d, void updateButtons())

private:
    class Private;
    Private *const d;
};

}

#endif
