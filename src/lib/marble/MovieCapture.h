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
#include <QVector>

#include "marble_export.h"

namespace Marble
{

class MarbleWidget;

class MovieCapturePrivate;

class MovieFormat
{
public:
    MovieFormat() {}
    explicit MovieFormat( const QString &type, const QString &name, const QString &extension) :
        m_type( type ),
        m_name( name ),
        m_extension( extension )
    {}

    QString type() const { return m_type; }
    QString name() const { return m_name; }
    QString extension() const { return m_extension; }
private:
    QString m_type;
    QString m_name;
    QString m_extension;
};

class MARBLE_EXPORT MovieCapture : public QObject
{
    Q_OBJECT
public:
    enum SnapshotMethod { TimeDriven, DataDriven };
    MovieCapture(MarbleWidget *widget, QObject *parent);
    ~MovieCapture();

    int fps() const;
    QString destination() const;
    QVector<MovieFormat> availableFormats();
    MovieCapture::SnapshotMethod snapshotMethod() const;
    bool checkToolsAvailability();

public Q_SLOTS:
    void setFps(int fps);
    void setFilename(const QString &path);
    void setSnapshotMethod(MovieCapture::SnapshotMethod method);
    void recordFrame();
    bool startRecording();
    void stopRecording();
    void cancelRecording();

private Q_SLOTS:
    void processWrittenMovie(int exitCode);

Q_SIGNALS:
    void rateCalculated( double );
    void errorOccured();

protected:
    MovieCapturePrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(MovieCapture)
    QVector<MovieFormat> m_supportedFormats;

};

} // namespace Marble

Q_DECLARE_TYPEINFO(Marble::MovieFormat, Q_MOVABLE_TYPE);

#endif // MOVIECAPTURE_H
