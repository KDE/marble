#ifndef KMLOBJECTPARSER_H
#define KMLOBJECTPARSER_H

#include <QtXml/QXmlDefaultHandler>

class KMLObjectParser : public QXmlDefaultHandler
{
 protected:
    KMLObjectParser();
};

#endif // KMLOBJECTPARSER_H
