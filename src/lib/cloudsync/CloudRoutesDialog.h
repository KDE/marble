//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CLOUDROUTESDIALOG_H
#define CLOUDROUTESDIALOG_H

#include "MarbleWidget.h"
#include "CloudRouteModel.h"

#include <QDialog>
#include <QListView>
#include <QStyledItemDelegate>

namespace Marble {

class CloudRoutesDialog : public QDialog
{
    Q_OBJECT
    
public:
    CloudRoutesDialog( CloudRouteModel *model, QWidget *parent = 0 );
    ~CloudRoutesDialog();
    CloudRouteModel *model();

public slots:
    void updateListDownloadProgressbar( qint64 received, qint64 total );

signals:
    void downloadButtonClicked( QString timestamp );
    void openButtonClicked( QString timestamp );
    void deleteButtonClicked( QString timestamp );
    void removeFromCacheButtonClicked( QString timestamp );
    void uploadToCloudButtonClicked( QString timestamp );

private:
    class Private;
    Private *d;

private slots:
    void updateNoRouteLabel();
};

}
#endif // CLOUDROUTESDIALOG_H
