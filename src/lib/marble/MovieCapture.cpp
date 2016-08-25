//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// Copyright 2014   Dennis Nienhüser <nienhueser@kde.org>
//

#include "MovieCapture.h"
#include "MarbleWidget.h"
#include "MarbleDebug.h"

#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QFile>

namespace Marble
{

class MovieCapturePrivate
{
public:
    explicit MovieCapturePrivate(MarbleWidget *widget) :
        marbleWidget(widget), method(MovieCapture::TimeDriven)
    {}

    /**
     * @brief This gets called when user doesn't have avconv/ffmpeg installed
     */
    void missingToolsWarning() {
        QMessageBox::warning(marbleWidget,
                             QObject::tr("Missing encoding tools"),
                             QObject::tr("Marble requires additional software in order to "
                                         "create movies. Please get %1 "
                                        ).arg("<a href=\"https://libav.org/"
                                               "download.html\">avconv</a>"),
                             QMessageBox::Ok);
    }

    QTimer frameTimer;
    MarbleWidget *marbleWidget;
    QString encoderExec;
    QString destinationFile;
    QProcess process;
    MovieCapture::SnapshotMethod method;
    int fps;
};

MovieCapture::MovieCapture(MarbleWidget *widget, QObject *parent) :
    QObject(parent),
    d_ptr(new MovieCapturePrivate(widget))
{
    Q_D(MovieCapture);
    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.setInterval(1000/30); // fps = 30 (default)
        connect(&d->frameTimer, SIGNAL(timeout()), this, SLOT(recordFrame()));
    }
    d->fps = 30;
    MovieFormat avi( "avi", tr( "AVI (mpeg4)" ), "avi" );
    MovieFormat flv( "flv", tr( "FLV" ), "flv" );
    MovieFormat mkv( "matroska", tr( "Matroska (h264)" ), "mkv" );
    MovieFormat mp4( "mp4", tr( "MPEG-4" ), "mp4" );
    MovieFormat vob( "vob", tr( "MPEG-2 PS (VOB)" ), "vob" );
    MovieFormat ogg( "ogg", tr( "OGG" ), "ogg" );
    MovieFormat swf( "swf", tr( "SWF" ), "swf" );
    m_supportedFormats << avi << flv << mkv << mp4 << vob << ogg << swf;
}

MovieCapture::~MovieCapture()
{
    delete d_ptr;
}

void MovieCapture::setFps(int fps)
{
    Q_D(MovieCapture);
    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.setInterval(1000/fps);
    }
    d->fps = fps;
}

void MovieCapture::setFilename(const QString &path)
{
    Q_D(MovieCapture);
    d->destinationFile = path;
}

void MovieCapture::setSnapshotMethod(MovieCapture::SnapshotMethod method)
{
    Q_D(MovieCapture);
    d->method = method;
}

int MovieCapture::fps() const
{
    Q_D(const MovieCapture);
    return d->fps;
}

QString MovieCapture::destination() const
{
    Q_D(const MovieCapture);
    return d->destinationFile;
}

QVector<MovieFormat> MovieCapture::availableFormats()
{
    Q_D(MovieCapture);
    static QVector<MovieFormat> availableFormats;
    if ( availableFormats.isEmpty() && checkToolsAvailability() ) {
        QProcess encoder(this);
        foreach ( const MovieFormat &format, m_supportedFormats ) {
            QString type = format.type();
            QStringList args;
            args << "-h" << QLatin1String("muxer=") + type;
            encoder.start( d->encoderExec, args );
            encoder.waitForFinished();
            QString output = encoder.readAll();
            bool isFormatAvailable = !output.contains(QLatin1String("Unknown format"));
            if( isFormatAvailable ) {
                availableFormats << format;
            }
        }
    }
    return availableFormats;
}

MovieCapture::SnapshotMethod MovieCapture::snapshotMethod() const
{
    Q_D(const MovieCapture);
    return d->method;
}

bool MovieCapture::checkToolsAvailability()
{
    Q_D(MovieCapture);
    static bool toolsAvailable = false;
    if (toolsAvailable == false) {
        QProcess encoder(this);
        encoder.start("avconv -version");
        encoder.waitForFinished();
        if ( !encoder.readAll().isEmpty() ) { // avconv have output when it's here
            d->encoderExec = "avconv";
            toolsAvailable = true;
        } else {
            encoder.start("ffmpeg -version");
            encoder.waitForFinished();
            if ( !encoder.readAll().isEmpty() ) {
                d->encoderExec = "ffmpeg";
                toolsAvailable = true;
            }
        }
    }
    return toolsAvailable;
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
    d->process.write( (char*) screenshot.bits(), screenshot.byteCount() );

    for (int i=0; i<30 && d->process.bytesToWrite()>0; ++i) {
        QTime t;
        int then = d->process.bytesToWrite();
        t.start();
        d->process.waitForBytesWritten( 100 );
        int span = t.elapsed();
        int now = d->process.bytesToWrite();
        int bytesWritten = then - now;
        double rate = ( bytesWritten * 1000.0 ) / ( qMax(1, span) * 1024 );
        emit rateCalculated( rate );
    }
}

bool MovieCapture::startRecording()
{
    Q_D(MovieCapture);

    if( !checkToolsAvailability() ) {
        d->missingToolsWarning();
        return false;
    }

    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.start();
    }
    recordFrame();
    return true;
}

void MovieCapture::stopRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->process.closeWriteChannel();
}

void MovieCapture::cancelRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->process.close();
    QFile::remove( d->destinationFile );
}

void MovieCapture::processWrittenMovie(int exitCode)
{
    if (exitCode != 0) {
        mDebug() << "[*] avconv finished with" << exitCode;
        emit errorOccured();
    }
}

} // namespace Marble

#include "moc_MovieCapture.cpp"
