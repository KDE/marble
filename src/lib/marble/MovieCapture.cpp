//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include <QDir>
#include <QDateTime>
#include <QProcess>
#include <QThread>
#include <QElapsedTimer>

#include "MovieCapture.h"
#include "MarbleWidget.h"
#include "MarbleDebug.h"

namespace Marble
{

class MovieCapturePrivate
{
public:
    MovieCapturePrivate(MarbleWidget *widget) :
        marbleWidget(widget),
        frameCount(0),
        sessionId(0)
    {}

    QTimer frameTimer;
    MarbleWidget *marbleWidget;
    int frameCount;
    QDir tempDir;
    QString encoderExec;
    QString destinationFile;
    int sessionId;
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

    QString numFormat = QString("%1").arg(d->frameCount, 6, 10, QChar('0'));
    QString fileFormat = QString("marble-%1-frame-%2.bmp").arg(d->sessionId).arg(numFormat);
    QString path = d->tempDir.absoluteFilePath(fileFormat);

    d->marbleWidget->mapScreenShot().save(path, "BMP");
    d->frameCount++;
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

    d->sessionId = qAbs(QDateTime::currentMSecsSinceEpoch());

    d->tempDir = QDir::temp();
    d->tempDir.mkdir(QString::number(d->sessionId));
    d->tempDir.cd(QString::number(d->sessionId));

    d->frameTimer.start();
    recordFrame();
}

void MovieCapture::stopRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->frameCount = 0;

    QProcess *avconv = new QProcess(this);
    QString input = QString("%1/marble-%2-frame-%06d.bmp")
            .arg(d->tempDir.path()).arg(d->sessionId);
    QString output = d->destinationFile;
    QString argv = QString("-i %1 -r %2 -b 2000k -y %3")
            .arg(input).arg(fps()).arg(output);

    connect(avconv, SIGNAL(finished(int)), this, SLOT(processWrittenMovie(int)));
    avconv->start(d->encoderExec+' '+argv);
}

void MovieCapture::processWrittenMovie(int exitCode)
{
    if (exitCode != 0)
        mDebug() << "[*] avconv finished with" << exitCode;

    Q_D(MovieCapture);
    foreach(const QString &bmp,
            d->tempDir.entryList(QStringList() << "*.bmp", QDir::Files)) {
        d->tempDir.remove(bmp);
    }
    d->tempDir.cdUp();
    d->tempDir.rmdir(QString::number(d->sessionId));
}

} // namespace Marble

#include "MovieCapture.moc"
