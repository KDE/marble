#ifndef ABSTRACTFILEVIEWITEM_H
#define ABSTRACTFILEVIEWITEM_H

#include <QtGlobal>

class AbstractFileViewItem
{
  public:
    virtual ~AbstractFileViewItem() {};

    virtual void saveFile() = 0;
    virtual void closeFile() = 0;
};

#endif
