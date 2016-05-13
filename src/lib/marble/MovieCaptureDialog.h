//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MOVIECAPTUREDIALOG_H
#define MOVIECAPTUREDIALOG_H

#include <QDialog>

#include "marble_export.h"

namespace Ui {
class MovieCaptureDialog;
}

namespace Marble {

class MarbleWidget;

class MovieCapture;
class MARBLE_EXPORT MovieCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieCaptureDialog(MarbleWidget *widget, QWidget *parent = 0);
    ~MovieCaptureDialog();

public Q_SLOTS:
    void startRecording();
    void stopRecording();

private Q_SLOTS:
    void loadDestinationFile();

Q_SIGNALS:
    void started();

private:
    Ui::MovieCaptureDialog *ui;
    MovieCapture *m_recorder;
};

} // namespace Marble

#endif // MOVIECAPTUREDIALOG_H
