#ifndef MARBLETEXTEDIT_H
#define MARBLETEXTEDIT_H

#include <QTextEdit>

class MarbleTextEdit : public QTextEdit
{
public:
    MarbleTextEdit( QWidget* parent );

protected:
    virtual void resizeEvent ( QResizeEvent * event );
};

#endif // MARBLETEXTEDIT_H
