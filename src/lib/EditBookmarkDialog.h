//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>     
//

#ifndef MARBLE_BOOKMARKINFODIALOG_H
#define MARBLE_BOOKMARKINFODIALOG_H

#include "ui_EditBookmarkDialog.h"
#include "marble_export.h"
#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"

/** @todo FIXME after freeze: Rename to AddBookmarkDialog*/

namespace Marble
{

class EditBookmarkDialogPrivate;
class BookmarkManager;
class GeoDataLookAt;
class MarbleWidget;

class MARBLE_EXPORT EditBookmarkDialog : public QDialog, private Ui::UiEditBookmarkDialog
{
    Q_OBJECT

 public:
    explicit EditBookmarkDialog( BookmarkManager *manager, QWidget *parent = 0 );

    /** Destructor */
    ~EditBookmarkDialog();

    void setMarbleWidget( MarbleWidget* widget );

    GeoDataPlacemark bookmark() const;

    QString name() const;

    QString folderName() const;

    QString description() const;

    GeoDataCoordinates coordinates() const;

    void setName( const QString &name );

    void setLookAt( const GeoDataLookAt &lookAt );

    void setDescription( const QString &text );

    void setFolderName( const QString &name );

 private Q_SLOTS:
    void addBookmark();
    
    void openNewFolderDialog();

    void retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark);

    QString append( const QString &bookmark, const QString &text);

    void onCoordinatesEdited();

 private:
    Q_DISABLE_COPY( EditBookmarkDialog )
    EditBookmarkDialogPrivate* const d;
    friend class EditBookmarkDialogPrivate;
};

}
#endif
