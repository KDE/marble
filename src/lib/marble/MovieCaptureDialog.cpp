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
    const QString destination =
            QFileDialog::getSaveFileName(this, tr("Save video file"), "",
                                         tr("Supported video (*.mp4 *.webm *.ogg)"));

    if (destination.isEmpty()) {
        return;
    }

    QStringList formats;
    formats << "mp4" << "webm" << "ogg";

    bool supported = false;
    foreach(const QString &format, formats) {
        if (destination.endsWith('.'+format))
            supported = true;
    }

    if (!supported) {
        QMessageBox::warning(this, tr("Filename is not valid"),
                             tr("This file format is not supported. "
                                "Please, use .ogg, .mp4 or .webm instead"));
        return;
    }

    ui->destinationEdit->setText(destination);
    m_recorder->setDestination(destination);
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
    m_recorder->startRecording();
    emit started();
}

void MovieCaptureDialog::stopRecording()
{
    m_recorder->stopRecording();
}

} // namespace Marble

#include "MovieCaptureDialog.moc"
