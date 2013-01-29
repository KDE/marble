//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>

class QLineEdit;
class QToolButton;
class QTextEdit;
class QHBoxLayout;
class QVBoxLayout;

namespace Marble
{

class GeoDataPlacemark;

class TextEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TextEditor( GeoDataPlacemark *placemark );
    ~TextEditor();

public Q_SLOTS:
    void updateName();
    void updateDescription();

private:
    GeoDataPlacemark *m_placemark;

    QAction* m_boldAction;
    QAction* m_underLineAction;
    QAction* m_italicAction;

    QToolButton* m_boldButton;
    QToolButton* m_underLineButton;
    QToolButton* m_italicButton;

    QLineEdit* m_name;
    QTextEdit* m_description;

    QHBoxLayout* m_buttonLayout;
    QVBoxLayout* m_layout;
};

}

#endif // TEXTEDITOR_H
