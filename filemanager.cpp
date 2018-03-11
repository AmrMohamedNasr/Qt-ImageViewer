#include "filemanager.h"

FileManager::FileManager()
{

}

bool FileManager::load_image(const QString &fileName, QString &err_message, QImage &img) {
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        err_message = reader.errorString();
        return false;
    }
    img = newImage;
    return true;
}

bool FileManager::save_image(const QString &fileName, QString &err_message, QImage &img) {
    QImageWriter writer(fileName);
    if (!writer.write(img)) {
        err_message = writer.errorString();
        return false;
    }
    return true;
}
