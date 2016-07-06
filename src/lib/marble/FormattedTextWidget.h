//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru <crucerucalincristian@gmail.com>
// Copyright 2015      Constantin Mihalache <mihalache.c94@gmail.com>
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
     explicit FormattedTextWidget(QWidget *parent = 0);
    ~FormattedTextWidget();

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
