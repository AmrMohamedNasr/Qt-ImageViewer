#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QtWidgets>
#include "filemanager.h"
#include <QWidget>
#include <QMainWindow>
#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QGraphicsView;
class QScrollBar;
class FileManager;
QT_END_NAMESPACE
class ImageScene;

class ImageProcessor
{
public:
    ImageProcessor();
    void setImage(const QImage &newImage, QGraphicsView * graphicsArea);
    void scaleImage(qreal factor, QGraphicsView * graphicsArea);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void rotateImage(qreal degree, bool temp_rotation);
    void cropImage(QRect selected_area, QGraphicsView * graphicsArea);
    void open(QMainWindow * parent, QGraphicsView * graphicsArea);
    void save(QMainWindow * parent);
    void zoomInOnSelected(QRect selected_area, QGraphicsView * graphicsArea);
    void resetZoom(QGraphicsView * graphicsArea);
    FileManager file_manager;
    QImage image;
    ImageScene& scene;
    QPixmap map;
    qreal scaleFactor;
    qreal current_degree;
    QGraphicsPixmapItem * pixmapItem;
    QPixmap rotPix;
};

#endif // IMAGEPROCESSOR_H
