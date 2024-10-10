// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2013 Roman Karlstetter <roman.karlstetter@googlemail.com>
//

#ifndef ELEVATIONPROFILECONTEXTMENU_H
#define ELEVATIONPROFILECONTEXTMENU_H

#include <QMenu>
#include <QObject>
#include <QSignalMapper>

namespace Marble
{

class ElevationProfileFloatItem;
class ElevationProfileContextMenu : public QObject
{
    Q_OBJECT
public:
    explicit ElevationProfileContextMenu(ElevationProfileFloatItem *floatItem);
    QMenu *getMenu();
public Q_SLOTS:
    void updateContextMenuEntries();

private:
    QList<QAction *> m_selectionActions;
    ElevationProfileFloatItem *m_floatItem = nullptr;
    QActionGroup *m_sourceGrp = nullptr;
    QMenu *m_contextMenu = nullptr;
    QSignalMapper *m_trackMapper = nullptr;
};
}
#endif
