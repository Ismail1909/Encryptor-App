#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include <QObject>
#include <QDebug>
#include <QFile>

#define KEYSIZE 32
#define IVSIZE 16
#define BLOCKSIZE 128
#define SALTSIZE 16

class Encryptor : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(type name READ name WRITE setName NOTIFY nameChanged)
public:
    explicit Encryptor(QObject *parent = nullptr);
    ~Encryptor();

    /**
     * @brief Encrypt a byte array with AES 256 CBC
     * @param data The byte array to encrypt
     * @return QByteArray
     */
    QByteArray encryptAES(QByteArray password, QByteArray &data);

    /**
     * @brief Decrypt a byte array with AES 256 CBC
     * @param data The byte array to decrypt
     * @return QByteArray
     */
    QByteArray decryptAES(QByteArray password, QByteArray &data);

private:

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
