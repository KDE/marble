// SPDX-License-Identifier: LGPL-2.1-or-later
//
// The code in this file is largely based on KDE's KLineEdit class
// as included in KDE 4.5. See there for its authors:
// https://api.kde.org/4.x-api/kdelibs-apidocs/kdeui/html/klineedit_8cpp.html
//
// SPDX-FileCopyrightText: 2010, 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MARBLELINEEDIT_H
#define MARBLE_MARBLELINEEDIT_H

#include "marble_export.h"

#include <QLineEdit>

namespace Marble
{

class MarbleLineEditPrivate;

/**
 * A QLineEdit with an embedded clear button. The clear button removes any input
 * in the line edit when clicked with the left mouse button. It replaces the line
 * edit input with the current clipboard content on a middle mouse button click.
 */
class MARBLE_EXPORT MarbleLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /** Constructor */
    explicit MarbleLineEdit(QWidget *parent = nullptr);

    /** Destructor */
    ~MarbleLineEdit() override;

    void setDecorator(const QPixmap &decorator);

    void setBusy(bool busy);

Q_SIGNALS:
    /** The clear button was clicked with the left mouse button */
    void clearButtonClicked();

    void decoratorButtonClicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void updateClearButtonIcon(const QString &text);

    void updateClearButton();

    void updateProgress();

private:
    MarbleLineEditPrivate *const d;
};

} // namespace Marble

#endif // MARBLE_MARBLELINEEDIT_H
