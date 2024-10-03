// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 David Roberts <dvdr18@gmail.com>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_SUNCONTROLWIDGET_H
#define MARBLE_SUNCONTROLWIDGET_H

#include <QDialog>

#include "marble_export.h"

namespace Ui
{
class SunControlWidget;
}

namespace Marble
{
class MarbleWidget;

class MARBLE_EXPORT SunControlWidget : public QDialog
{
    Q_OBJECT

public:
    explicit SunControlWidget(MarbleWidget *marbleWidget, QWidget *parent = nullptr);
    ~SunControlWidget() override;
    void setSunShading(bool);

private Q_SLOTS:
    void apply();

Q_SIGNALS:
    void showSun(bool show);
    void isLockedToSubSolarPoint(bool show);
    void isSubSolarPointIconVisible(bool show);

protected:
    Q_DISABLE_COPY(SunControlWidget)

    void showEvent(QShowEvent *event) override;

    Ui::SunControlWidget *const m_uiWidget;
    MarbleWidget *const m_marbleWidget;
    QString m_shadow;
};

}

#endif
