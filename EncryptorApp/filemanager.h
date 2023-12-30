#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QFile>
#include <QSaveFile>

class FileManager: public QObject
{
    Q_OBJECT
public:
    FileManager(QObject *parent = nullptr);
    //~FileManager();


};

#endif // FILEMANAGER_H
