//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014   Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef TOURCAPTUREDIALOG_H
#define TOURCAPTUREDIALOG_H

#include <QDialog>

#include "marble_export.h"

namespace Ui {
class TourCaptureDialog;
}

namespace Marble {

class MarbleWidget;

class MovieCapture;
class TourPlayback;
class MARBLE_EXPORT TourCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TourCaptureDialog(MarbleWidget *widget, QWidget *parent = 0);
    ~TourCaptureDialog();

public Q_SLOTS:
    void startRecording();
    void stopRecording();
    void setRate( double rate );
    void setTourPlayback( TourPlayback* playback );
    void setDefaultFilename( const QString &filename );
    void handleError();

private Q_SLOTS:
    void loadDestinationFile();
    void updateProgress( double position );
    void recordNextFrame();

private:
    Ui::TourCaptureDialog *ui;
    MovieCapture *m_recorder;
    TourPlayback *m_playback;
    bool m_writingPossible;
    double m_current_position;
    QString m_defaultFileName;
};

} // namespace Marble

#endif // MOVIECAPTUREDIALOG_H
