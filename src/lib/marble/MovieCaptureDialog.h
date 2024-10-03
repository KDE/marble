// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MOVIECAPTUREDIALOG_H
#define MOVIECAPTUREDIALOG_H

#include <QDialog>

#include "marble_export.h"

namespace Ui
{
class MovieCaptureDialog;
}

namespace Marble
{

class MarbleWidget;

class MovieCapture;
class MARBLE_EXPORT MovieCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MovieCaptureDialog(MarbleWidget *widget, QWidget *parent = nullptr);
    ~MovieCaptureDialog() override;

public Q_SLOTS:
    void startRecording();
    void stopRecording();

private Q_SLOTS:
    void loadDestinationFile();

Q_SIGNALS:
    void started();

private:
    Ui::MovieCaptureDialog *const ui;
    MovieCapture *const m_recorder;
};

} // namespace Marble

#endif // MOVIECAPTUREDIALOG_H
