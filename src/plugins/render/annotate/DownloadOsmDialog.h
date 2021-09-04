//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Sutirtha Ghosh <ghsutirtha@gmail.com>
//

#ifndef DOWNLOADOSMDIALOG_H
#define DOWNLOADOSMDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <GeoDataLatLonAltBox.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QTemporaryFile>


#include "LatLonBoxWidget.h"
#include "ui_DownloadOsmDialog.h"

namespace Marble
{
class MarbleWidget;
class AnnotatePlugin;
class DownloadOsmDialog : public QDialog,private Ui::DownloadOsmDialog
{
    Q_OBJECT

public:
    explicit DownloadOsmDialog(MarbleWidget *parent = nullptr,AnnotatePlugin *annotatePlugin = nullptr);
    ~DownloadOsmDialog() override;

Q_SIGNALS:
    void openFile(const QString &filename);

private:
    void updateCoordinates();
    MarbleWidget *m_marbleWidget;
    QPushButton *m_downloadButton;
    QNetworkAccessManager m_qnam;
    QNetworkReply *m_reply;
    QTemporaryFile *m_file;
    LatLonBoxWidget *m_latLonBoxWidget;
    bool m_isDownloadSuccess;

private Q_SLOTS:
    void downloadFile();
    void updateCoordinates(const GeoDataLatLonAltBox&);
    void httpReadyRead();
    void httpFinished();
};

}
#endif // DOWNLOADOSMDIALOG_H
