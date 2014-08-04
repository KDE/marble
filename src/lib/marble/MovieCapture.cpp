//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// Copyright 2014   Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MovieCapture.h"
#include "MarbleWidget.h"
#include "MarbleDebug.h"

#include <QProcess>

namespace Marble
{

class MovieCapturePrivate
{
public:
    MovieCapturePrivate(MarbleWidget *widget) :
        marbleWidget(widget)
    {}

    QTimer frameTimer;
    MarbleWidget *marbleWidget;
    QString encoderExec;
    QString destinationFile;
    QProcess process;
};

MovieCapture::MovieCapture(MarbleWidget *widget, QObject *parent) :
    QObject(parent),
    d_ptr(new MovieCapturePrivate(widget))
{
    Q_D(MovieCapture);
    d->frameTimer.setInterval(1000/30); // fps = 30 (default)
    connect(&d->frameTimer, SIGNAL(timeout()), this, SLOT(recordFrame()));
}

MovieCapture::~MovieCapture()
{
    delete d_ptr;
}

void MovieCapture::setFps(int fps)
{
    Q_D(MovieCapture);
    d->frameTimer.setInterval(1000/fps);
}

void MovieCapture::setDestination(const QString &path)
{
    Q_D(MovieCapture);
    d->destinationFile = path;
}

int MovieCapture::fps() const
{
    Q_D(const MovieCapture);
    return 1000/d->frameTimer.interval();
}

QString MovieCapture::destination() const
{
    Q_D(const MovieCapture);
    return d->destinationFile;
}

void MovieCapture::recordFrame()
{
    Q_D(MovieCapture);
    QImage const screenshot = d->marbleWidget->mapScreenShot().toImage().convertToFormat(QImage::Format_RGB888);
    if (d->process.state() == QProcess::NotRunning) {
        QStringList const arguments = QStringList()
                << "-y"
                << "-r" << QString::number(fps())
                << "-f" << "rawvideo"
                << "-pix_fmt" << "rgb24"
                << "-s" << QString("%1x%2").arg( screenshot.width() ).arg( screenshot.height() )
                << "-i" << "pipe:"
                << "-b" << "2000k"
                << d->destinationFile;
        d->process.start( d->encoderExec, arguments );
        connect(&d->process, SIGNAL(finished(int)), this, SLOT(processWrittenMovie(int)));
    }
    d->process.write( (char*) screenshot.bits(), screenshot.numBytes() );
}

void MovieCapture::startRecording()
{
    Q_D(MovieCapture);

    static bool toolsAvailable = false;
    if (toolsAvailable == false) {
        QProcess encoder(this);
        encoder.start("avconv -version");
        encoder.waitForFinished();
        if ( !encoder.readAll().isEmpty() ) { // avconv have output when it's here
            d->encoderExec = "avconv";
        } else {
            encoder.start("ffmpeg -version");
            encoder.waitForFinished();
            if ( !encoder.readAll().isEmpty() ) {
                d->encoderExec = "ffmpeg";
            } else {
                return;
            }
        }
    }

    d->frameTimer.start();
    recordFrame();
}

void MovieCapture::stopRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->process.close();
}

void MovieCapture::processWrittenMovie(int exitCode)
{
    if (exitCode != 0) {
        qDebug() << "[*] avconv finished with" << exitCode;
    }
}

} // namespace Marble

#include "MovieCapture.moc"
