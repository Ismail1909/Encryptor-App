#include "encryptor.h"


#include <string.h>

#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/core.h>
#include <openssl/core_names.h>     /* OSSL_KDF_*           */
#include <openssl/params.h>         /* OSSL_PARAM_*         */
#include <openssl/thread.h>         /* OSSL_set_max_threads */
#include <openssl/kdf.h>            /* EVP_KDF_*            */

typedef struct evp_kdf_st EVP_KDF;
typedef struct evp_kdf_ctx_st EVP_KDF_CTX;


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

bool Encryptor::encryptAES(QString filename)
{

    /* Define encryption key and initialization vector(for CBC) */
    unsigned char key[KEYSIZE];
    unsigned char iv[IVSIZE];



    QFile inFile(filename);

    RAND_bytes(iv,IVSIZE);

    QByteArray buffer = QByteArray(reinterpret_cast<char*>(iv), IVSIZE);

    if(tempFile.open())
    {
        tempFile.write("AES__");
        tempFile.write(buffer);
    }

    kdf_deriveArgon(iv,key);

    /* Initialize OpenSSL Cipher Envelope */
    EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(en);

    /* Initialize OpenSSL Cipher Encryption Envelope for AES 256-bit with CBC */
    if(!EVP_EncryptInit_ex(en, EVP_aes_256_cbc(),NULL,key, iv))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);
        return false;
    }


    /* Final encrypted data length (encrypted + salt) */
    int f_len = 0;



    /* Recheck the Encryption engine is working */
    if(!EVP_EncryptInit_ex(en, NULL, NULL, NULL, NULL))
    {
        qCritical() << "EVP_EncryptInit_ex() failed " << ERR_error_string(ERR_get_error(), NULL);

        return false;
    }

    if(inFile.open(QIODevice::ReadOnly)){
        QTextStream stream1(&inFile);
        QTextStream stream0(&tempFile);
        char FileData[1024];


        //stream0 << "AES__" << iv ;

        while(inFile.atEnd() == false )
        {

            int len = inFile.read(FileData,1024);
            /* Set a pointer to the plain data and its length */
            char *input = FileData;
            /* Encrypted data length */
            int c_len = len + AES_BLOCK_SIZE;

            /* Allocating memory for encrypted data */
            unsigned char *ciphertext = (unsigned char*)malloc(c_len);

            /* Encrypts bytes from the buffer input and writes the encrypted version to ciphertext */
            if(!EVP_EncryptUpdate(en, ciphertext, &c_len,(const unsigned char *)input, len))
            {
                qCritical() << "EVP_EncryptUpdate() failed " << ERR_error_string(ERR_get_error(), NULL);
                free(ciphertext);
                return false;
            }


                /* Encrypts the “final” data block and finishes the Encryption Operation */
                if(!EVP_EncryptFinal(en, ciphertext+c_len, &f_len))
                {
                    qCritical() << "EVP_EncryptFinal_ex() failed "  << ERR_error_string(ERR_get_error(), NULL);
                    free(ciphertext);
                    return false;
                }

                len = c_len + f_len;

                tempFile.write((const char*)ciphertext,len);
            free(ciphertext);

        }

        inFile.close();
        tempFile.close();

    }

    /* Clean up Envelope */
    EVP_CIPHER_CTX_cleanup(en);

    return true;

}

bool Encryptor::decryptAES(QString filename)
{
    QFile inFile(filename);


    if(inFile.open(QIODevice::ReadOnly))
    {

        if(QString(inFile.read(5)) != "AES__")
        {
            return false;
        }

        /* Define encryption key and initialization vector(for CBC) */
        unsigned char key[KEYSIZE];
        unsigned char iv[IVSIZE];

        inFile.read((char*)iv,16);

        kdf_deriveArgon(iv,key);

        /* Initialize OpenSSL Cipher Envelope */
        EVP_CIPHER_CTX* de = EVP_CIPHER_CTX_new();
        EVP_CIPHER_CTX_init(de);

        /* Initialize OpenSSL Cipher Decryption Envelope for AES 256-bit with CBC */
        if(!EVP_DecryptInit_ex(de,EVP_aes_256_cbc(), NULL, key,iv ))
        {
            qCritical() << "EVP_DecryptInit_ex() failed" << ERR_error_string(ERR_get_error(), NULL);
            return false;
        }

        if(tempFile.open()){

            char FileData[1024];

            while(inFile.atEnd() == false  )
            {
                int len = inFile.read(FileData,1024);
                /* Set a pointer to the plain data and its length */
                char *input = FileData;
                /* Encrypted data length */
                int p_len = len, f_len =0;

                /* Allocating memory for encrypted data */
                unsigned char *plaintext = (unsigned char*)malloc(p_len + AES_BLOCK_SIZE);

                /* Decrypts bytes from the buffer input and writes the decrypted version to plaintext */
                if(!EVP_DecryptUpdate(de, plaintext, &p_len, (const unsigned char *)input, len))
                {
                    qCritical() << "EVP_DecryptUpdate() failed " <<  ERR_error_string(ERR_get_error(), NULL);
                    free(plaintext);
                    return false;
                }

                /* Encrypts the “final” data block and finishes the Decryption Operation */
                if(inFile.atEnd() == true){
                    if(!EVP_DecryptFinal_ex(de,plaintext+p_len,&f_len))
                    {
                        qCritical() << "EVP_DecryptFinal_ex() failed " <<  ERR_error_string(ERR_get_error(), NULL);
                        free(plaintext);
                        return false;
                    }
                }

                len = p_len + f_len;

                tempFile.write((const char*)plaintext,len);
                free(plaintext);

            }

            tempFile.close();
        }

        /* Clean up Envelope */
        EVP_CIPHER_CTX_cleanup(de);

        inFile.close();
    }


    return true;
}

void Encryptor::setPassword(QString _password)
{
    this->password = _password.toLatin1();
}

void Encryptor::saveFile(QString filename)
{
    QFile file(filename);
    if(!tempFile.open())
    {
        qCritical() << tempFile.errorString();
        return;
    }

    if(!file.open(QFile::WriteOnly))
    {
        qCritical() << file.errorString();
        return;
    }


    while(tempFile.atEnd() != true)
    {
        file.write(tempFile.read(1024));
    }

    file.close();
    tempFile.close();
    tempFile.remove();
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

bool Encryptor::kdf_deriveArgon(unsigned char salt[], unsigned char key[])
{
    bool ret = true;
    EVP_KDF *kdf = NULL;
    EVP_KDF_CTX *kctx = NULL;
    OSSL_PARAM params[6];
    OSSL_PARAM *p = params;
    unsigned char pwd[password.length()];

    /* derive result */
    size_t keylen = 256;

    /* argon2 params, please refer to RFC9106 for recommended defaults */
    uint32_t lanes = 2, threads = 2, memcost = 65536;
    for(int i=0;i<password.length();i++)
    {
        pwd[i] = password[i];
    }

    if (OSSL_set_max_threads(NULL, threads) != 1)
        ret = false;


    p = params;
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_THREADS, &threads);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_LANES,
                                       &lanes);
    *p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ARGON2_MEMCOST,
                                       &memcost);
    //*p++ = OSSL_PARAM_construct_uint32(OSSL_KDF_PARAM_ITER, &iter);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, salt, 16);
    *p++ = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_PASSWORD, pwd, password.length());
    *p++ = OSSL_PARAM_construct_end();

    if ((kdf = EVP_KDF_fetch(NULL, "ARGON2D", NULL)) == NULL)
        ret = false;

    if ((kctx = EVP_KDF_CTX_new(kdf)) == NULL)
        ret = false;

    if (EVP_KDF_derive(kctx, key, keylen, params) != 1)
        ret = false;


    EVP_KDF_free(kdf);
    EVP_KDF_CTX_free(kctx);
    OSSL_set_max_threads(NULL, 0);


    return ret;

}





