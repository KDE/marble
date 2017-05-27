//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Sutirtha Ghosh  <ghsutirtha@gmail.com>
//

#ifndef DOWNLOADOSMDIALOG_H
#define DOWNLOADOSMDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <GeoDataLatLonAltBox.h>
#include <QtNetwork>
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
    explicit DownloadOsmDialog(MarbleWidget *parent = 0,AnnotatePlugin *annotatePlugin = 0);
    ~DownloadOsmDialog();

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
