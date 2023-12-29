#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QObject>

class Encryptor : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)
public:
    explicit Encryptor(QObject *parent = nullptr);

signals:

};

#endif // ENCRYPTOR_H
