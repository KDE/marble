// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MOVIECAPTURE_H
#define MOVIECAPTURE_H

#include <QList>
#include <QObject>

#include "marble_export.h"

namespace Marble
{

class MarbleWidget;

class MovieCapturePrivate;

class MovieFormat
{
public:
    MovieFormat() = default;
    explicit MovieFormat(const QString &type, const QString &name, const QString &extension)
        : m_type(type)
        , m_name(name)
        , m_extension(extension)
    {
    }

    QString type() const
    {
        return m_type;
    }
    QString name() const
    {
        return m_name;
    }
    QString extension() const
    {
        return m_extension;
    }

private:
    QString m_type;
    QString m_name;
    QString m_extension;
};

class MARBLE_EXPORT MovieCapture : public QObject
{
    Q_OBJECT
public:
    enum SnapshotMethod {
        TimeDriven,
        DataDriven
    };
    MovieCapture(MarbleWidget *widget, QObject *parent);
    ~MovieCapture() override;

    int fps() const;
    QString destination() const;
    QList<MovieFormat> availableFormats();
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
    void rateCalculated(double);
    void errorOccured();

protected:
    MovieCapturePrivate *const d_ptr;

private:
    Q_DECLARE_PRIVATE(MovieCapture)
    QList<MovieFormat> m_supportedFormats;
};

} // namespace Marble

Q_DECLARE_TYPEINFO(Marble::MovieFormat, Q_MOVABLE_TYPE);

#endif // MOVIECAPTURE_H
