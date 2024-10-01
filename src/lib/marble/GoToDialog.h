// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_GOTODIALOG_H
#define MARBLE_GOTODIALOG_H

#include "marble_export.h"

#include <QDialog>

class QModelIndex;

namespace Marble
{

class GeoDataPlacemark;
class GeoDataCoordinates;
class GoToDialogPrivate;
class MarbleModel;

/**
 * A dialog that provides a list of targets to go to: The current location,
 * route start, destination and via points and bookmarks. Clicking on a target
 * centers the map on the selected position
 */
class MARBLE_EXPORT GoToDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GoToDialog(MarbleModel *marbleModel, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    ~GoToDialog() override;

    /** Toggle whether routing items (source, destination and via points) are visible */
    void setShowRoutingItems(bool show);

    /** Toggle whether the dialog can be used to search for placemarks */
    void setSearchEnabled(bool enabled);

    /**
     * Returns the position of the item selected by the user, or a default
     * constructed GeoDataLookAt if the dialog was cancelled by the user
     */
    GeoDataCoordinates coordinates() const;

    Q_PRIVATE_SLOT(d, void saveSelection(const QModelIndex &index))

    Q_PRIVATE_SLOT(d, void startSearch())

    Q_PRIVATE_SLOT(d, void updateSearchResult(const QList<GeoDataPlacemark *> &placemarks))

    Q_PRIVATE_SLOT(d, void updateSearchMode())

    Q_PRIVATE_SLOT(d, void updateProgress())

    Q_PRIVATE_SLOT(d, void stopProgressAnimation())

private:
    GoToDialogPrivate *const d;
    friend class GoToDialogPrivate;
};

} // namespace Marble

#endif // MARBLE_GOTODIALOG_H
