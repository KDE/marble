// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>

#ifndef MEASURECONFIGDIALOG_H
#define MEASURECONFIGDIALOG_H

#include <QDialog>
#include "ui_MeasureConfigDialog.h"
#include "MeasureToolPlugin.h"

namespace Marble {

class MeasureConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MeasureConfigDialog(QDialog *parent = nullptr);
    ~MeasureConfigDialog() override;

    bool showBearingLabel() const;
    bool showBearingLabelChange() const;
    bool showDistanceLabels() const;
    bool showRadius() const;
    bool showPerimeter() const;
    bool showCircumference() const;
    bool showPolygonArea() const;
    bool showCircularArea() const;
    MeasureToolPlugin::PaintMode paintMode() const;

    void setShowBearingLabel(bool);
    void setShowBearingLabelChange(bool);
    void setShowDistanceLabels(bool);
    void setShowRadius(bool);
    void setShowPerimeter(bool);
    void setShowCircumference(bool);
    void setShowPolygonArea(bool);
    void setShowCircularArea(bool);
    void setPaintMode(MeasureToolPlugin::PaintMode);

private Q_SLOTS:
    void updateTabs();

Q_SIGNALS:
    void applied();

private:
    Ui::MeasureConfigDialog *ui;
};

} // namespace Marble

#endif // MEASURECONFIGDIALOG_H
