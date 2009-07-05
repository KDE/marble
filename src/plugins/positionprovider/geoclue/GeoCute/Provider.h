#ifndef GEOCUTE_PROVIDER_H
#define GEOCUTE_PROVIDER_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "Status.h"



namespace GeoCute {

class Provider : public QObject {
    Q_OBJECT
    
    public:
        Provider(const QString& service, const QString& path,
            QObject* parent = 0);
        ~Provider();
        Status status() const;

    signals:
        void statusChanged(GeoCute::Status status);
    
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void statusChangedCall(int status))
};

}



#endif
