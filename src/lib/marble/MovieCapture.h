//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MOVIECAPTURE_H
#define MOVIECAPTURE_H

#include <QObject>
#include <QTimer>
#include <QDir>

#include "marble_export.h"

namespace Marble
{

class MarbleWidget;

class MovieCapturePrivate;
class MARBLE_EXPORT MovieCapture : public QObject
{
    Q_OBJECT
public:
    MovieCapture(MarbleWidget *widget, QObject *parent);
    ~MovieCapture();

    int fps() const;
    QString destination() const;

public slots:
    void setFps(int fps);
    void setDestination(const QString &path);

    void startRecording();
    void stopRecording();

private slots:
    void processWrittenMovie(int exitCode);

private slots:
    void recordFrame();

protected:
    MovieCapturePrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(MovieCapture)

};

} // namespace Marble

#endif // MOVIECAPTURE_H
