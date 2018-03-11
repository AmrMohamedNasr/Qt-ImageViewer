#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QtWidgets>
#include <QImage>

class FileManager
{
public:
    FileManager();
    bool load_image(const QString &fileName, QString &err_message, QImage &img);
    bool save_image(const QString &fileName, QString &err_message, QImage &img);
};

#endif // FILEMANAGER_H
