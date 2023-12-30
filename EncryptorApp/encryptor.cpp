#include "encryptor.h"

#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define KEY_ROUNDS 1

Encryptor::Encryptor(QObject *parent)
    : QObject{parent}
{
    initalize();
}

Encryptor::~Encryptor()
{
    finalize();
}

QByteArray Encryptor::encryptAES(QByteArray password, QByteArray &data)
{
    /* Initialze random salt, it's important for preventing the creation of a pattern
       in the files encrypted with the same password */
    QByteArray msalt = randomBytes(SALTSIZE);

    /* Key Derivation rounds, more rounds = more secure but less perfomance */
    int rounds = KEY_ROUNDS;

    /* Define encryption key and initialization vector(for CBC) */
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];

    const unsigned char* salt = (const unsigned char*) msalt.constData();
    const unsigned char* _password = (const unsigned char*) password.constData();

    /* This function generates secure 256 bit key from the salt and the user password */
    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt,_password, password.length(),rounds,key,iv);

    if(i != KEYSIZE)
    {
        /* Key Generation Failed */
        qCritical() << "EVP_BytesToKey() error: " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Initialize OpenSSL Cipher Envelope */
    EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(en);

    /* Initialize OpenSSL Cipher Encryption Envelope for AES 256-bit with CBC */
    if(!EVP_EncryptInit_ex(en, EVP_aes_256_cbc(),NULL,key, iv))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Set a pointer to the plain data and its length */
    char *input = data.data();
    int len = data.size();

    /* Encrypted data length */
    int c_len = len + AES_BLOCK_SIZE;
    /* Final encrypted data length (encrypted + salt) */
    int f_len = 0;

    /* Allocating memory for encrypted data */
    unsigned char *ciphertext = (unsigned char*)malloc(c_len);

    /* Recheck the Encryption engine is working */
    if(!EVP_EncryptInit_ex(en, NULL, NULL, NULL, NULL))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Encrypts bytes from the buffer input and writes the encrypted version to ciphertext */
    if(!EVP_EncryptUpdate(en, ciphertext, &c_len,(unsigned char *)input, len))
    {
        qCritical() << "EVP_EncryptUpdate() failed " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Encrypts the “final” data block and finishes the Encryption Operation */
    if(!EVP_EncryptFinal(en, ciphertext+c_len, &f_len))
    {
        qCritical() << "EVP_EncryptFinal_ex() failed "  << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    len = c_len + f_len;

    /* Clean up Envelope */
    EVP_CIPHER_CTX_cleanup(en);

    QByteArray encrypted = QByteArray(reinterpret_cast<char*>(ciphertext), len);
    QByteArray finished;

    /* Append the salt to the encrypted data to be retrieved by the decryptor */
    finished.append("Salted__");
    finished.append(msalt);
    finished.append(encrypted);

    free(ciphertext);

    return finished;

}

QByteArray Encryptor::decryptAES(QByteArray password, QByteArray &data)
{
    /* Retrieve the salt generated from encryption */
    QByteArray msalt;
    if(QString(data.mid(0,8)) == "Salted__")
    {
        msalt = data.mid(8,SALTSIZE);
        data = data.mid(8+SALTSIZE);
    }
    else
    {
        qWarning() << "Could not load salt from data!";
        msalt = randomBytes(SALTSIZE);
    }

    int rounds = KEY_ROUNDS;

    /* Define encryption key and initialization vector(for CBC) */
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];

    const unsigned char* salt = (const unsigned char*)msalt.constData();
    const unsigned char* _password = (const unsigned char*)password.data();

    /* This function generates secure 256 bit key from the salt and the user password */
    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt,_password, password.length(),rounds,key,iv);

    if(i != KEYSIZE)
    {
        /* Key Generation Failed */
        qCritical() << "EVP_BytesToKey() error: " << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Initialize OpenSSL Cipher Envelope */
    EVP_CIPHER_CTX* de = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(de);

    /* Initialize OpenSSL Cipher Decryption Envelope for AES 256-bit with CBC */
    if(!EVP_DecryptInit_ex(de,EVP_aes_256_cbc(), NULL, key,iv ))
    {
        qCritical() << "EVP_DecryptInit_ex() failed" << ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    /* Set a pointer to the encrypted data and its length */
    char *input = data.data();
    int len = data.size();


    int p_len = len, f_len = 0;

    /* Allocating memory for decrypted data */
    unsigned char *plaintext = (unsigned char *)malloc(p_len + AES_BLOCK_SIZE);

    /* Decrypts bytes from the buffer input and writes the decrypted version to plaintext */
    if(!EVP_DecryptUpdate(de, plaintext, &p_len, (unsigned char *)input, len))
    {
        qCritical() << "EVP_DecryptUpdate() failed " <<  ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

     /* Encrypts the “final” data block and finishes the Decryption Operation */
    if(!EVP_DecryptFinal_ex(de,plaintext+p_len,&f_len))
    {
        qCritical() << "EVP_DecryptFinal_ex() failed " <<  ERR_error_string(ERR_get_error(), NULL);
        return QByteArray();
    }

    len = p_len + f_len;

    /* Clean up Envelope */
    EVP_CIPHER_CTX_cleanup(de);


    QByteArray decrypted = QByteArray(reinterpret_cast<char*>(plaintext), len);
    free(plaintext);

    return decrypted;

}

void Encryptor::initalize()
{
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
}

void Encryptor::finalize()
{
    EVP_cleanup();
    ERR_free_strings();
}

QByteArray Encryptor::randomBytes(int size)
{
    unsigned char arr[size];
    RAND_bytes(arr,size);

    QByteArray buffer = QByteArray(reinterpret_cast<char*>(arr), size);
    return buffer;
}





