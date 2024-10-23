// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MovieCapture.h"
#include "MarbleDebug.h"
#include "MarbleWidget.h"

#include <QElapsedTimer>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

namespace Marble
{

class MovieCapturePrivate
{
public:
    explicit MovieCapturePrivate(MarbleWidget *widget)
        : marbleWidget(widget)
        , method(MovieCapture::TimeDriven)
    {
    }

    /**
     * @brief This gets called when user doesn't have avconv/ffmpeg installed
     */
    void missingToolsWarning()
    {
        QMessageBox::warning(marbleWidget,
                             QObject::tr("Missing encoding tools"),
                             QObject::tr("Marble requires additional software in order to "
                                         "create movies. Please get %1 ")
                                 .arg(QStringLiteral("<a href=\"https://libav.org/"
                                                     "download.html\">avconv</a>")),
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

MovieCapture::MovieCapture(MarbleWidget *widget, QObject *parent)
    : QObject(parent)
    , d_ptr(new MovieCapturePrivate(widget))
{
    Q_D(MovieCapture);
    if (d->method == MovieCapture::TimeDriven) {
        d->frameTimer.setInterval(1000 / 30); // fps = 30 (default)
        connect(&d->frameTimer, &QTimer::timeout, this, &MovieCapture::recordFrame);
    }
    d->fps = 30;
    MovieFormat avi(QStringLiteral("avi"), tr("AVI (mpeg4)"), QStringLiteral("avi"));
    MovieFormat flv(QStringLiteral("flv"), tr("FLV"), QStringLiteral("flv"));
    MovieFormat mkv(QStringLiteral("matroska"), tr("Matroska (h264)"), QStringLiteral("mkv"));
    MovieFormat mp4(QStringLiteral("mp4"), tr("MPEG-4"), QStringLiteral("mp4"));
    MovieFormat vob(QStringLiteral("vob"), tr("MPEG-2 PS (VOB)"), QStringLiteral("vob"));
    MovieFormat ogg(QStringLiteral("ogg"), tr("OGG"), QStringLiteral("ogg"));
    MovieFormat swf(QStringLiteral("swf"), tr("SWF"), QStringLiteral("swf"));
    m_supportedFormats << avi << flv << mkv << mp4 << vob << ogg << swf;
}

MovieCapture::~MovieCapture()
{
    delete d_ptr;
}

void MovieCapture::setFps(int fps)
{
    Q_D(MovieCapture);
    if (d->method == MovieCapture::TimeDriven) {
        d->frameTimer.setInterval(1000 / fps);
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

QList<MovieFormat> MovieCapture::availableFormats()
{
    Q_D(MovieCapture);
    static QList<MovieFormat> availableFormats;
    if (availableFormats.isEmpty() && checkToolsAvailability()) {
        QProcess encoder(this);
        for (const MovieFormat &format : std::as_const(m_supportedFormats)) {
            QString type = format.type();
            QStringList args;
            args << QStringLiteral("-h") << QLatin1StringView("muxer=") + type;
            encoder.start(d->encoderExec, args);
            encoder.waitForFinished();
            QString output = QString::fromLatin1(encoder.readAll());
            bool isFormatAvailable = !output.contains(QLatin1StringView("Unknown format"));
            if (isFormatAvailable) {
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
        encoder.start(QStringLiteral("avconv"), QStringList() << QStringLiteral("-version"));
        encoder.waitForFinished();
        if (!encoder.readAll().isEmpty()) { // avconv have output when it's here
            d->encoderExec = QStringLiteral("avconv");
            toolsAvailable = true;
        } else {
            encoder.start(QStringLiteral("ffmpeg"), QStringList() << QStringLiteral("-version"));
            encoder.waitForFinished();
            if (!encoder.readAll().isEmpty()) {
                d->encoderExec = QStringLiteral("ffmpeg");
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
        QStringList const arguments = QStringList() << QStringLiteral("-y") << QStringLiteral("-r") << QString::number(fps()) << QStringLiteral("-f")
                                                    << QStringLiteral("rawvideo") << QStringLiteral("-pix_fmt") << QStringLiteral("rgb24")
                                                    << QStringLiteral("-s") << QStringLiteral("%1x%2").arg(screenshot.width()).arg(screenshot.height())
                                                    << QStringLiteral("-i") << QStringLiteral("pipe:") << QStringLiteral("-b") << QStringLiteral("2000k")
                                                    << d->destinationFile;
        d->process.start(d->encoderExec, arguments);
        connect(&d->process, SIGNAL(finished(int)), this, SLOT(processWrittenMovie(int)));
    }
    d->process.write((char *)screenshot.bits(), screenshot.sizeInBytes());
    for (int i = 0; i < 30 && d->process.bytesToWrite() > 0; ++i) {
        QElapsedTimer t;
        int then = d->process.bytesToWrite();
        t.start();
        d->process.waitForBytesWritten(100);
        int span = t.elapsed();
        int now = d->process.bytesToWrite();
        int bytesWritten = then - now;
        double rate = (bytesWritten * 1000.0) / (qMax(1, span) * 1024);
        Q_EMIT rateCalculated(rate);
    }
}

bool MovieCapture::startRecording()
{
    Q_D(MovieCapture);

    if (!checkToolsAvailability()) {
        d->missingToolsWarning();
        return false;
    }

    if (d->method == MovieCapture::TimeDriven) {
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
    QFile::remove(d->destinationFile);
}

void MovieCapture::processWrittenMovie(int exitCode)
{
    if (exitCode != 0) {
        mDebug() << "[*] avconv finished with" << exitCode;
        Q_EMIT errorOccured();
    }
}

} // namespace Marble

#include "moc_MovieCapture.cpp"
