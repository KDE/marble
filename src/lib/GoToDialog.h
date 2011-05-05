//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_GOTODIALOG_H
#define MARBLE_GOTODIALOG_H

#include "marble_export.h"
#include "GeoDataLookAt.h"

#include <QtGui/QDialog>
#include <QtGui/QRadioButton>

#include "ui_GoToDialog.h"

class QModelIndex;

namespace Marble
{

class GoToDialogPrivate;
class MarbleWidget;

/**
  * A dialog that provides a list of targets to go to: The current location,
  * route start, destination and via points and bookmarks. Clicking on a target
  * centers the map on the selected position
  */
class MARBLE_EXPORT GoToDialog: public QDialog, private Ui::GoTo
{
    Q_OBJECT

public:
    explicit GoToDialog( MarbleWidget* marbleWidget, QWidget * parent = 0, Qt::WindowFlags f = 0 );

    ~GoToDialog();

    /** Toggle whether routing items (source, destination and via points) are visible */
    void setShowRoutingItems( bool show );

    /**
      * Returns the position of the item selected by the user, or a default
      * constructed GeoDataLookAt if the dialog was cancelled by the user
      */
    GeoDataLookAt lookAt() const;

    Q_PRIVATE_SLOT( d, void saveSelection( const QModelIndex &index ) )

private Q_SLOTS:
    void startSearch();

    void updateSearchResult( QAbstractItemModel* model );

    void updateSearchMode();

private:
    GoToDialogPrivate * const d;
};

} // namespace Marble

#endif // MARBLE_GOTODIALOG_H
