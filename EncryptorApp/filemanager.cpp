#include "filemanager.h"
#include "qdebug.h"


FileManager::FileManager(QObject *parent) : QObject{parent}
{


}

FileManager::~FileManager()
{

}

QString FileManager::getExtension(QString filename)
{
    QFileInfo fi(filename);

    return fi.completeSuffix();
}

qint64 FileManager::getSize(QString filename)
{
    QFileInfo fi(filename);


    return fi.size();

}



QByteArray FileManager::readFile(QString filename)
{
    QByteArray data;
    QFile file(filename);
    if(!file.open(QFile::ReadOnly))
    {
        qCritical() << file.errorString();
        return data;
    }

    data = file.readAll();
    file.close();
    return data;
}


void FileManager::saveFile(QString filename)
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


    file.write(tempFile.readAll());

    tempFile.remove();
}

void FileManager::writeTempFile(const QByteArray& data)
{
    if(!tempFile.open())
    {
        qCritical() << tempFile.errorString();
        return;
    }

    tempFile.write(data);
    tempFile.close();

}


