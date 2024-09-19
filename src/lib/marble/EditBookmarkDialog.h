// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//

#ifndef MARBLE_EDITBOOKMARKDIALOG_H
#define MARBLE_EDITBOOKMARKDIALOG_H

#include <QDialog>

#include "marble_export.h"

/** @todo FIXME after freeze: Rename to AddBookmarkDialog*/

namespace Marble
{

class EditBookmarkDialogPrivate;
class BookmarkManager;
class GeoDataFolder;
class GeoDataCoordinates;
class GeoDataPlacemark;
class MarbleWidget;

class MARBLE_EXPORT EditBookmarkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditBookmarkDialog(BookmarkManager *manager, QWidget *parent = nullptr);

    /** Destructor */
    ~EditBookmarkDialog() override;

    void setMarbleWidget(MarbleWidget *widget);

    GeoDataPlacemark bookmark() const;

    QString name() const;

    GeoDataFolder *folder() const;

    QString description() const;

    GeoDataCoordinates coordinates() const;

    qreal range() const;

    QString iconLink() const;

    void setName(const QString &name);
    void setReverseGeocodeName();

    void setCoordinates(const GeoDataCoordinates &coordinates);

    void setRange(qreal range);

    void setDescription(const QString &text);

    void setFolderName(const QString &name);

    void setIconLink(const QString &iconLink);

private:
    Q_PRIVATE_SLOT(d, void openNewFolderDialog())

    Q_PRIVATE_SLOT(d, void retrieveGeocodeResult(const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark))

    Q_PRIVATE_SLOT(d, void updateCoordinates())

    Q_DISABLE_COPY(EditBookmarkDialog)
    EditBookmarkDialogPrivate *const d;
    friend class EditBookmarkDialogPrivate;
};

}
#endif
