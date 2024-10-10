// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011-2012 Florian Eßer <f.esser@rwth-aachen.de>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2013 Roman Karlstetter <roman.karlstetter@googlemail.com>
//
#include "ElevationProfileContextMenu.h"

#include <QActionGroup>

#include "ElevationProfileDataSource.h"
#include "ElevationProfileFloatItem.h"

namespace Marble
{

ElevationProfileContextMenu::ElevationProfileContextMenu(ElevationProfileFloatItem *floatItem)
    : QObject(floatItem)
    , m_floatItem(floatItem)
{
}

QMenu *ElevationProfileContextMenu::getMenu()
{
    if (!m_contextMenu) {
        m_contextMenu = m_floatItem->contextMenu();

        for (QAction *action : m_contextMenu->actions()) {
            if (action->text() == tr("&Configure...")) {
                m_contextMenu->removeAction(action);
                break;
            }
        }

        QAction *zoomToViewportAction = m_contextMenu->addAction(tr("&Zoom to viewport"), m_floatItem, SLOT(toggleZoomToViewport()));
        zoomToViewportAction->setCheckable(true);
        zoomToViewportAction->setChecked(m_floatItem->m_zoomToViewport);
        m_contextMenu->addSeparator();

        m_sourceGrp = new QActionGroup(this);
        m_trackMapper = new QSignalMapper(this);
        updateContextMenuEntries();
    }
    return m_contextMenu;
}

void ElevationProfileContextMenu::updateContextMenuEntries()
{
    // context menu has not yet been initialized, this functions is called as last step of initialization
    if (!m_contextMenu) {
        return;
    }

    // completely rebuild selection, TODO could be possibly improved to only add/remove items incrementally
    for (QAction *action : m_selectionActions) {
        m_contextMenu->removeAction(action);
    }

    // clear state
    qDeleteAll(m_selectionActions);
    m_selectionActions.clear();

    // add route data source (if available)
    if (m_floatItem->m_routeDataSource.isDataAvailable()) {
        auto route = new QAction(tr("Route"), m_contextMenu);
        route->setActionGroup(m_sourceGrp);
        route->setCheckable(true);
        route->setChecked(m_floatItem->m_activeDataSource == &m_floatItem->m_routeDataSource);
        connect(route, &QAction::triggered, m_floatItem, &ElevationProfileFloatItem::switchToRouteDataSource);
        m_selectionActions.append(route);
    }

    // add tracks (if available)
    if (m_floatItem->m_trackDataSource.isDataAvailable()) {
        QStringList sources = m_floatItem->m_trackDataSource.sourceDescriptions();
        for (int i = 0; i < sources.size(); ++i) {
            auto track = new QAction(tr("Track: ") + sources[i], m_contextMenu);
            connect(track, SIGNAL(triggered()), m_trackMapper, SLOT(map()));
            track->setCheckable(true);
            track->setChecked(m_floatItem->m_activeDataSource == &m_floatItem->m_trackDataSource && m_floatItem->m_trackDataSource.currentSourceIndex() == i);
            track->setActionGroup(m_sourceGrp);
            m_selectionActions.append(track);
            m_trackMapper->setMapping(track, i);
        }
        connect(m_trackMapper, &QSignalMapper::mappedInt, m_floatItem, &ElevationProfileFloatItem::switchToTrackDataSource);
    }

    // no route or track available, add disabled action to inform user about it
    if (m_selectionActions.isEmpty()) {
        auto disabledInformationAction = new QAction(tr("Create a route or load a track from file to view its elevation profile."), m_contextMenu);
        disabledInformationAction->setEnabled(false);
        m_selectionActions.append(disabledInformationAction);
    }

    for (QAction *action : m_selectionActions) {
        m_contextMenu->addAction(action);
    }
}

}

#include "moc_ElevationProfileContextMenu.cpp"
