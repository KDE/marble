//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Sutirtha Ghosh  <ghsutirtha@gmail.com>
//

//Self
#include "DownloadOsmDialog.h"

//Qt
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTemporaryFile>
#include <QTemporaryFile>
#include  <QDir>

//Marble
#include "ui_DownloadOsmDialog.h"
#include "MarbleWidget.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLatLonBox.h"
#include "MarbleGlobal.h"
#include "AnnotatePlugin.h"
#include "LatLonBoxWidget.h"

namespace Marble
{
DownloadOsmDialog::DownloadOsmDialog(MarbleWidget *parent,AnnotatePlugin *annotatePlugin) :
    QDialog(parent),
    m_marbleWidget(parent),
    m_latLonBoxWidget(new LatLonBoxWidget)
{
    QPushButton *m_quitButton;
    setupUi(this);
    horizontalLayout->addWidget(m_latLonBoxWidget);
    this->setWindowTitle("Download");
    connect(m_marbleWidget,
            SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
            this,
            SLOT(updateCoordinates(GeoDataLatLonAltBox) )
            );

    m_downloadButton = new QPushButton(tr("Download"));
    m_downloadButton->setDefault(true);
    m_quitButton = new QPushButton(tr("Cancel"));
    m_quitButton->setAutoDefault(false);

    buttonBox->addButton(m_downloadButton,QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_quitButton,QDialogButtonBox::RejectRole);

    connect( m_downloadButton, SIGNAL(clicked(bool)), this, SLOT(downloadFile()) );
    connect( m_quitButton, SIGNAL(clicked(bool)), this, SLOT(close()) );
    connect( this, SIGNAL(openFile(QString)), annotatePlugin, SLOT(openAnnotationFile(QString)) );

    progressBar->hide();
    updateCoordinates();
}

DownloadOsmDialog::~DownloadOsmDialog()
{

}

void DownloadOsmDialog::updateCoordinates()
{

    m_latLonBoxWidget->setLatLonBox( m_marbleWidget->viewport()->viewLatLonAltBox() );
}

void DownloadOsmDialog::updateCoordinates( const GeoDataLatLonAltBox& boundingBox )
{
    m_latLonBoxWidget->setLatLonBox( boundingBox );
}

void DownloadOsmDialog::downloadFile()
{
    QString m_west;
    QString m_south;
    QString m_east;
    QString m_north;
    QString url;
    m_isDownloadSuccess=false;
    m_file = new QTemporaryFile(QDir::tempPath()+"/"+"XXXXXXosmdata.osm");
    if(!m_file->open())
    {
        QMessageBox::information(this, tr("ERROR"),
                                 "Unable to create temporary file to download OSM data to.");
        this->close();
    }
    m_downloadButton->setEnabled(false);

    m_west=QString::number( m_latLonBoxWidget->latLonBox().west()*RAD2DEG );
    m_south=QString::number( m_latLonBoxWidget->latLonBox().south()*RAD2DEG );
    m_east=QString::number( m_latLonBoxWidget->latLonBox().east()*RAD2DEG );
    m_north=QString::number( m_latLonBoxWidget->latLonBox().north()*RAD2DEG );

    url="http://api.openstreetmap.org/api/0.6/map?bbox=";
    url+=m_west+",";
    url+=m_south+",";
    url+=m_east+",";
    url+=m_north;

    m_reply = m_qnam.get(QNetworkRequest(QUrl(url)));

    connect( m_reply, SIGNAL(finished()), this, SLOT(httpFinished()) );
    connect( m_reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()) );

    progressBar->show();
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
}

void DownloadOsmDialog::httpReadyRead()
{
    //Reads all the data present and writes it to the file whenever data is
    //available
    if ( m_file ){
        m_file->write(m_reply->readAll());
    }
}

void DownloadOsmDialog::httpFinished()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    //in case download fails due to network error or server not available
    if ( m_reply->error()==QNetworkReply::NoError ){
        m_isDownloadSuccess=true;
    }
    else {
        switch(status)
        {
        case 400:
            QMessageBox::information(this, tr("ERROR"),
                                     "The selected region contains too much data.Please select a smaller region and try again.");
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess=false;
            break;
        case 509:
            QMessageBox::information(this, tr("ERROR"),
                                     "The bandwidth limit exceeded.Please try again later.");
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess=false;
            break;
        default:
            QMessageBox::information(this, tr("ERROR"),"Sorry, a network error occured.Please check your internet connection"
                                                       " or try again later.");
            m_downloadButton->setEnabled(true);
            m_isDownloadSuccess=false;
            break;
        }
    }

    progressBar->hide();
    m_file->flush();
    m_file->close();
    if( m_isDownloadSuccess ) {
        emit openFile(m_file->fileName());
    }
    m_reply->deleteLater();
    m_reply=0;
    delete m_file;
    m_file=0;
    if( m_isDownloadSuccess ) {
        this->close();
    }
}

}
#include "moc_DownloadOsmDialog.cpp"
