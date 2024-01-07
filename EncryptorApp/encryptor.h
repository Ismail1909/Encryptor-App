#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include "qqmlintegration.h"
#include <QObject>
#include <QDebug>
#include <QFile>
#include<QTemporaryFile>

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

    QTemporaryFile tempFile{"tempFile"};

    Encryptor(QObject *parent = nullptr);
    ~Encryptor();

    /**
     * @brief Encrypt a byte array with AES 256 CBC
     * @param data The byte array to encrypt
     * @return QByteArray
     */
    Q_INVOKABLE bool encryptAES(QString filename);

    /**
     * @brief Decrypt a byte array with AES 256 CBC
     * @param data The byte array to decrypt
     * @return QByteArray
     */
    Q_INVOKABLE bool decryptAES(QString filename);

    /**
     * @brief get the size of the file
     * @param file path or name
     * @return QString
     */
    Q_INVOKABLE void setPassword(QString _password);

    Q_INVOKABLE void saveFile(QString filename);

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


    bool kdf_deriveArgon(unsigned char salt[], unsigned char key[]);






signals:

};

#endif // ENCRYPTOR_H
