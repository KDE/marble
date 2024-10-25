// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef TOURCAPTUREDIALOG_H
#define TOURCAPTUREDIALOG_H

#include <QDialog>

#include "marble_export.h"

namespace Ui
{
class TourCaptureDialog;
}

namespace Marble
{

class MarbleWidget;

class MovieCapture;
class TourPlayback;
class MARBLE_EXPORT TourCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TourCaptureDialog(MarbleWidget *widget, QWidget *parent = nullptr);
    ~TourCaptureDialog() override;

public Q_SLOTS:
    void startRecording();
    void stopRecording();
    void setRate(double rate);
    void setTourPlayback(TourPlayback *playback);
    void setDefaultFilename(const QString &filename);
    void handleError();

private Q_SLOTS:
    void loadDestinationFile();
    void updateProgress(double position);
    void recordNextFrame();

private:
    Ui::TourCaptureDialog *const ui;
    MovieCapture *const m_recorder;
    TourPlayback *m_playback = nullptr;
    bool m_writingPossible;
    double m_current_position;
    QString m_defaultFileName;
};

} // namespace Marble

#endif // MOVIECAPTUREDIALOG_H
