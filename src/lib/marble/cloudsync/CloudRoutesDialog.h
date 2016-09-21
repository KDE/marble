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

#include <QDialog>

namespace Marble {

class CloudRouteModel;

class CloudRoutesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CloudRoutesDialog( CloudRouteModel *model, QWidget *parent = 0 );
    ~CloudRoutesDialog();
    CloudRouteModel *model();

public Q_SLOTS:
    void updateListDownloadProgressbar( qint64 received, qint64 total );

Q_SIGNALS:
    void downloadButtonClicked( const QString& timestamp );
    void openButtonClicked( const QString& timestamp );
    void deleteButtonClicked( const QString& timestamp );
    void removeFromCacheButtonClicked( const QString& timestamp );
    void uploadToCloudButtonClicked( const QString& timestamp );

private:
    class Private;
    Private *d;

private Q_SLOTS:
    void updateNoRouteLabel();
};

}
#endif // CLOUDROUTESDIALOG_H
