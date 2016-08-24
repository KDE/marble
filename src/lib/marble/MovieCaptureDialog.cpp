//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "MovieCaptureDialog.h"
#include "ui_MovieCaptureDialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "MarbleWidget.h"
#include "MovieCapture.h"

namespace Marble {

MovieCaptureDialog::MovieCaptureDialog(MarbleWidget *widget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MovieCaptureDialog),
    m_recorder(new MovieCapture(widget, parent))
{
    ui->setupUi(this);
    m_recorder->setSnapshotMethod(MovieCapture::TimeDriven);

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            ui->fpsSpin, SLOT(setValue(int)));

    connect(ui->fpsSpin, SIGNAL(valueChanged(int)),
            ui->fpsSlider, SLOT(setValue(int)));

    connect(ui->fpsSlider, SIGNAL(valueChanged(int)),
            m_recorder, SLOT(setFps(int)));

    connect(ui->cancelButton, SIGNAL(clicked()),
            this, SLOT(close()));

    connect(ui->startButton, SIGNAL(clicked()),
            this, SLOT(startRecording()));

    connect(ui->openButton, SIGNAL(clicked()),
            this, SLOT(loadDestinationFile()));
}

MovieCaptureDialog::~MovieCaptureDialog()
{
    delete ui;
}

void MovieCaptureDialog::loadDestinationFile()
{
    const QVector<MovieFormat> formats = m_recorder->availableFormats();
    if( formats.isEmpty() ) {
        QMessageBox::warning( this, tr( "Codecs are unavailable" ), tr( "Supported codecs are not found." ) );
        return;
    }
    QString filter = formats.first().name() + QLatin1String(" (*.") + formats.first().extension() + QLatin1Char(')');
    for( int i = 1; i < formats.size(); i++ )
    {
        filter += QLatin1String(";;") + formats.at(i).name() + QLatin1String(" (*.") + formats.at(i).extension() + QLatin1Char(')');
    }
    const QString defaultFileName =
            ui->destinationEdit->text().isEmpty() ? "" : ui->destinationEdit->text();

    const QString destination =
            QFileDialog::getSaveFileName(this, tr("Save video file"), defaultFileName,
                                         filter );

    if (destination.isEmpty()) {
        return;
    }

    bool supported = false;
    foreach(const MovieFormat &format, formats) {
        if (destination.endsWith(QLatin1Char('.') + format.extension())) {
            supported = true;
            break;
        }
    }

    if (!supported) {
        QString formatsExtensions = QLatin1Char('.') + formats.at(0).extension();
        for( int i = 1; i < formats.size(); ++i )
        {
            formatsExtensions += QLatin1String(", .") + formats.at(i).extension();
        }
        QMessageBox::warning(this, tr("Filename is not valid"),
                             tr("This file format is not supported. "
                                "Please, use %1 instead").arg( formatsExtensions ) );
        return;
    }

    ui->destinationEdit->setText(destination);
    m_recorder->setFilename(destination);
}

void MovieCaptureDialog::startRecording()
{
    const QString path = ui->destinationEdit->text();

    if (path.isEmpty()) {
        QMessageBox::warning(this, tr("Missing filename"),
                             tr("Destination video file is not set. "
                                "I don't know where to save recorded "
                                "video. Please, specify one."));
        return;
    }

    hide();

    m_recorder->setFps(ui->fpsSlider->value());
    if (m_recorder->startRecording())
        emit started();
}

void MovieCaptureDialog::stopRecording()
{
    m_recorder->stopRecording();
}

} // namespace Marble

#include "moc_MovieCaptureDialog.cpp"
