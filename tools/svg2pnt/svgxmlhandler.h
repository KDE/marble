#ifndef SVGXMLHANDLER_H
#define SVGXMLHHANDLER_H

#include <QXmlDefaultHandler>

class SVGXmlHandler : public QXmlDefaultHandler {
private:
	int m_header;
	int m_pointnum;
	bool m_initialized;
	QString m_targetfile;
public:
	SVGXmlHandler(const QString&);
//	virtual bool startDocument();
//	virtual bool endDocument();
	virtual bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
//	virtual bool endElement();
};

#endif // SEARCHCOMBOBOX_H
