// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Sutirtha Ghosh <ghsutirtha@gmail.com>
//

// Self
#include "DownloadOsmDialog.h"

// Qt
#include <QDialogButtonBox>
#include <QDir>
#include <QMessageBox>

// Marble
#include "AnnotatePlugin.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLatLonBox.h"
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
#include "ViewportParams.h"

namespace Marble
{
DownloadOsmDialog::DownloadOsmDialog(MarbleWidget *parent, AnnotatePlugin *annotatePlugin)
    : QDialog(parent)
    , m_marbleWidget(parent)
    , m_latLonBoxWidget(new LatLonBoxWidget(this))
{
    setupUi(this);
    horizontalLayout->addWidget(m_latLonBoxWidget);
    this->setWindowTitle(tr("Download"));
    connect(m_marbleWidget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)), this, SLOT(updateCoordinates(GeoDataLatLonAltBox)));

    m_downloadButton = new QPushButton(tr("Download"), this);
    m_downloadButton->setDefault(true);

    buttonBox->addButton(m_downloadButton, QDialogButtonBox::ActionRole);

    connect(m_downloadButton, &QAbstractButton::clicked, this, &DownloadOsmDialog::downloadFile);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QWidget::close);
    connect(this, SIGNAL(openFile(QString)), annotatePlugin, SLOT(openAnnotationFile(QString)));

    progressBar->hide();
    updateCoordinates();
}

DownloadOsmDialog::~DownloadOsmDialog() = default;

void DownloadOsmDialog::updateCoordinates()
{
    m_latLonBoxWidget->setLatLonBox(m_marbleWidget->viewport()->viewLatLonAltBox());
}

void DownloadOsmDialog::updateCoordinates(const GeoDataLatLonAltBox &boundingBox)
{
    m_latLonBoxWidget->setLatLonBox(boundingBox);
}

void DownloadOsmDialog::downloadFile()
{
    QString m_west;
    QString m_south;
    QString m_east;
    QString m_north;
    QString url;
    m_isDownloadSuccess = false;
    m_file = new QTemporaryFile(QDir::tempPath() + QStringLiteral("/XXXXXXosmdata.osm"));
    if (!m_file->open()) {
        QMessageBox::information(this, tr("ERROR"), tr("Unable to create temporary file to download OSM data to."));
        this->close();
    }
    m_downloadButton->setEnabled(false);

    m_west = QString::number(m_latLonBoxWidget->latLonBox().west() * RAD2DEG);
    m_south = QString::number(m_latLonBoxWidget->latLonBox().south() * RAD2DEG);
    m_east = QString::number(m_latLonBoxWidget->latLonBox().east() * RAD2DEG);
    m_north = QString::number(m_latLonBoxWidget->latLonBox().north() * RAD2DEG);

    url = QStringLiteral("http://api.openstreetmap.org/api/0.6/map?bbox=");
    url += m_west + QStringLiteral(",");
    url += m_south + QStringLiteral(",");
    url += m_east + QStringLiteral(",");
    url += m_north;

    m_reply = m_qnam.get(QNetworkRequest(QUrl(url)));

    connect(m_reply, &QNetworkReply::finished, this, &DownloadOsmDialog::httpFinished);
    connect(m_reply, &QIODevice::readyRead, this, &DownloadOsmDialog::httpReadyRead);

    progressBar->show();
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
}

void DownloadOsmDialog::httpReadyRead()
{
    // Reads all the data present and writes it to the file whenever data is
    // available
    if (m_file) {
        m_file->write(m_reply->readAll());
    }
}

void DownloadOsmDialog::httpFinished()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    // in case download fails due to network error or server not available
    if (m_reply->error() == QNetworkReply::NoError) {
        m_isDownloadSuccess = true;
    } else {
        switch (status) {
        case 400:
            QMessageBox::information(this, tr("ERROR"), tr("The selected region contains too much data. Please select a smaller region and try again."));
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess = false;
            break;
        case 509:
            QMessageBox::information(this, tr("ERROR"), tr("The bandwidth limit exceeded. Please try again later."));
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess = false;
            break;
        default:
            QMessageBox::information(this,
                                     tr("ERROR"),
                                     tr("Sorry, a network error occurred. Please check your internet connection"
                                        " or try again later."));
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess = false;
            break;
        }
    }

    progressBar->hide();
    m_file->flush();
    m_file->close();
    if (m_isDownloadSuccess) {
        Q_EMIT openFile(m_file->fileName());
    }
    m_reply->deleteLater();
    m_reply = nullptr;
    delete m_file;
    m_file = nullptr;
    if (m_isDownloadSuccess) {
        this->close();
    }
}

}
#include "moc_DownloadOsmDialog.cpp"
