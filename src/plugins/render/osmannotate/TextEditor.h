//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
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

class TextEditor : public QWidget
{
public:
    TextEditor();
    ~TextEditor();

    //return the plain text name
    QString name() const;
    void setName( const QString &name );
    //return the HTML description
    QString description() const;
    void setDescription( const QString &description );

private:
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
