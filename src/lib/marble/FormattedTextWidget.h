// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
// SPDX-FileCopyrightText: 2015 Constantin Mihalache <mihalache.c94@gmail.com>
//

#ifndef FORMATTEDTEXTWIDGET_H
#define FORMATTEDTEXTWIDGET_H

// Marble
#include "marble_export.h"

//Qt
#include <QWidget>

class QString;

namespace Marble {

class MARBLE_EXPORT FormattedTextWidget : public QWidget
{
    Q_OBJECT

public:
     explicit FormattedTextWidget(QWidget *parent = nullptr);
    ~FormattedTextWidget() override;

     void setText( const QString &text );
     const QString text();
     void setReadOnly( bool state );

private Q_SLOTS:
     void toggleDescriptionEditMode( bool isFormattedTextMode = false );
     void setTextCursorBold( bool bold );
     void setTextCursorItalic( bool italic );
     void setTextCursorUnderlined( bool underlined );
     void setTextCursorColor( const QColor &color );
     void setTextCursorFont( const QFont &font );
     void setTextCursorFontSize( const QString &fontSize );
     void addImageToDescription();
     void addLinkToDescription();
     void updateDescriptionEditButtons();

Q_SIGNALS:
     void textUpdated();

private:
    class Private;
    Private * const d;
};
}

#endif // FORMATTEDTEXTWIDGET_H
