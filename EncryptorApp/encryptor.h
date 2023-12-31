#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include "qqmlintegration.h"
#include <QObject>
#include <QDebug>
#include <QFile>

#define KEYSIZE 32
#define IVSIZE 16
#define BLOCKSIZE 128
#define SALTSIZE 16

class Encryptor : public QObject
{
    /* Class used as a QML Singleton */
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit Encryptor(QObject *parent = nullptr);
    ~Encryptor();

    /**
     * @brief Encrypt a byte array with AES 256 CBC
     * @param data The byte array to encrypt
     * @return QByteArray
     */
    Q_INVOKABLE QByteArray encryptAES(const QByteArray &data);

    /**
     * @brief Decrypt a byte array with AES 256 CBC
     * @param data The byte array to decrypt
     * @return QByteArray
     */
    Q_INVOKABLE QByteArray decryptAES(const QByteArray &data);

    /**
     * @brief get the size of the file
     * @param file path or name
     * @return QString
     */
    Q_INVOKABLE void setPassword(const QByteArray password);

private:

    QByteArray password;

    /**
      * @brief Initalize the OpenSSL Lib
      */
    void initalize();

    /**
      * @brief Cleanup after the OpenSSL lib
      */
    void finalize();

    /**
     * @brief Get a byte array filled with random information
     * @param size The size of the byte array to generate
     * @return QByteArray
     */
    QByteArray randomBytes(int size);






signals:

};

#endif // ENCRYPTOR_H
