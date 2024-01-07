#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "qqmlintegration.h"
#include <QObject>
#include <QFile>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>


class FileManager: public QObject
{
    /* Class used as a QML Singleton */
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    FileManager(QObject *parent = nullptr);
    ~FileManager();

    /**
     * @brief get the Extension of the file
     * @param file path or name
     * @return QString
     */
    Q_INVOKABLE QString getExtension(QString filename);

    /**
     * @brief get the size of the file
     * @param file path or name
     * @return QString
     */
    Q_INVOKABLE qint64 getSize(QString filename);

    /**
     * @brief get the file in array of bytes
     * @param file path or name
     * @return QByteArray
     */
    Q_INVOKABLE QByteArray readFile(QString filename);

    /**
     * @brief write data to a temporary file
     * @param data to write
     * @return void
     */
    Q_INVOKABLE void writeTempFile(const QByteArray& data);

    /**
     * @brief save Temporary file to specified destination file
     * @param Destination file path or name
     * @return void
     */
    Q_INVOKABLE void saveFile(QString filename);

private:
    /* Temporary File Object */
    QTemporaryFile tempFile;



};

#endif // FILEMANAGER_H
