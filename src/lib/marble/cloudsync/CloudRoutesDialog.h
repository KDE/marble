// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef CLOUDROUTESDIALOG_H
#define CLOUDROUTESDIALOG_H

#include <QDialog>

namespace Marble
{

class CloudRouteModel;

class CloudRoutesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CloudRoutesDialog(CloudRouteModel *model, QWidget *parent = nullptr);
    ~CloudRoutesDialog() override;
    CloudRouteModel *model();

public Q_SLOTS:
    void updateListDownloadProgressbar(qint64 received, qint64 total);

Q_SIGNALS:
    void downloadButtonClicked(const QString &timestamp);
    void openButtonClicked(const QString &timestamp);
    void deleteButtonClicked(const QString &timestamp);
    void removeFromCacheButtonClicked(const QString &timestamp);
    void uploadToCloudButtonClicked(const QString &timestamp);

private:
    class Private;
    Private *const d;

private Q_SLOTS:
    void updateNoRouteLabel();
};

}
#endif // CLOUDROUTESDIALOG_H
