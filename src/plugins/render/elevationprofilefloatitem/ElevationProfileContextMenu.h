//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013      Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#ifndef ELEVATIONPROFILECONTEXTMENU_H
#define ELEVATIONPROFILECONTEXTMENU_H

#include <QObject>
#include <QSignalMapper>
#include <QMenu>


namespace Marble
{

class ElevationProfileFloatItem;
class ElevationProfileContextMenu : public QObject {
    Q_OBJECT
public:
    explicit ElevationProfileContextMenu(ElevationProfileFloatItem* floatItem);
    QMenu* getMenu();
public Q_SLOTS:
    void updateContextMenuEntries();

private:
    QList<QAction*>            m_selectionActions;
    ElevationProfileFloatItem* m_floatItem;
    QActionGroup*              m_sourceGrp;
    QMenu*                     m_contextMenu;
    QSignalMapper*             m_trackMapper;

};
}
#endif
