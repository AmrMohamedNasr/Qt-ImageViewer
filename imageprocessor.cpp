#include "imageprocessor.h"
#include <QMessageBox>
#include "imagescene.h"

ImageProcessor::ImageProcessor():
scene(*(new ImageScene(NULL)))
{
    scene.clear();
    pixmapItem = scene.addPixmap(map);
}

void ImageProcessor::setImage(const QImage &newImage, QGraphicsView * graphicsArea) {
    image = newImage;
    QPixmap temp = QPixmap::fromImage(newImage);
    rotPix = temp;
    map.swap(temp);
    scene.clear();
    pixmapItem = scene.addPixmap(map);
    scene.setSceneRect(map.rect());
    scaleFactor = 1.0;
    current_degree = 0;
}

void ImageProcessor::scaleImage(qreal factor, QGraphicsView * graphicsArea) {
    QPixmap orgPix = QPixmap::fromImage(image);
    scaleFactor *= factor;
    //qreal rotated_width = orgPix.height() * sin(current_degree) + orgPix.width() * cos(current_degree);
    //qreal rotated_height = orgPix.height() * cos(current_degree) + orgPix.width() * sin(current_degree);
    map = rotPix.scaled(scaleFactor * rotPix.size(),Qt::KeepAspectRatio,  Qt::FastTransformation);
    scene.clear();
    pixmapItem = scene.addPixmap(map);
    scene.setSceneRect(map.rect());
    adjustScrollBar(graphicsArea->horizontalScrollBar(), factor);
    adjustScrollBar(graphicsArea->verticalScrollBar(), factor);
}

void ImageProcessor::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ImageProcessor::rotateImage(qreal degree, bool temp_rotation) {
    QPixmap pixmap(QPixmap::fromImage(image));
    pixmap = pixmap.scaled(scaleFactor * pixmap.size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    QTransform rm;
    rm.translate(pixmap.rect().width() / 2, pixmap.rect().height() / 2);
    qreal rotAng = current_degree + degree;
    if (rotAng > 360) {
        rotAng -= 360;
    }
    rm.rotate(rotAng);
    rm.translate(-pixmap.rect().width() / 2, -pixmap.rect().height() / 2);
    pixmap = pixmap.transformed(rm);
    map.swap(pixmap);
    scene.clear();
    pixmapItem = scene.addPixmap(map);
    scene.setSceneRect(map.rect());
    if (!temp_rotation) {
        current_degree += degree;
        if (current_degree > 360) {
            current_degree -= 360;
        }
        QTransform rm2;
        pixmap = QPixmap::fromImage(image);
        rm2.translate(pixmap.rect().width() / 2, pixmap.rect().height() / 2);
        rm2.rotate(current_degree);
        rm2.translate(-pixmap.rect().width() / 2, -pixmap.rect().height() / 2);
        rotPix = pixmap.transformed(rm2);
    }
}

void ImageProcessor::cropImage(QRect selected_area, QGraphicsView * graphicsArea) {
    QPoint a = selected_area.topLeft();
    QPoint b = selected_area.bottomRight();
    if (a.x() > b.x()) {
        int temp_x = a.x();
        a.setX(b.x());
        b.setX(temp_x);
    }
    if (a.y() > b.y()) {
        int temp_y = a.y();
        a.setY(b.y());
        b.setY(temp_y);
    }
    QPixmap originalPix(map);
    double sx = map.rect().width();
    double sy = map.rect().height();
    sx = originalPix.width() / sx;
    sy = originalPix.height() / sy;
    QPoint a_m(int(a.x() * sx), int(a.y() * sy));
    QPoint b_m(int(b.x() * sx), int(b.y() * sy));
    QRect myRect(a_m, b_m);
    QPixmap cropped = originalPix.copy(myRect);
    setImage(cropped.toImage(), graphicsArea);
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode) {
    static bool firstDialog = true;
    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    mimeTypeFilters.append("image/bmp");
    mimeTypeFilters.append("image/png");
    mimeTypeFilters.append("image/jpeg");
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave) {
        dialog.setAcceptMode(QFileDialog::AcceptSave);
    } else {
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
    }
}

static void openImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode) {
    static bool firstDialog = true;
    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    dialog.setNameFilter(QObject::tr("Images (*.png *.xpm *.jpg *.JPG *.PNG *.XMP)"));
    if (acceptMode == QFileDialog::AcceptSave) {
        dialog.setAcceptMode(QFileDialog::AcceptSave);
    } else {
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
    }
}

void ImageProcessor::open(QMainWindow * parent, QGraphicsView * graphicsArea) {
    QFileDialog dialog(parent, QObject::tr("Open File"));
    openImageFileDialog(dialog, QFileDialog::AcceptOpen);
    bool load = false;
    QString err_mesg, fileName;
    while (!load && dialog.exec() == QDialog::Accepted) {
        fileName = dialog.selectedFiles().first();
       load = file_manager.load_image(fileName, err_mesg, image);
        if (!load) {
             QMessageBox::information(parent, QGuiApplication::applicationDisplayName(),
                 QObject::tr("Cannot load %1: %2")
                 .arg(QDir::toNativeSeparators(fileName), err_mesg));
        }
    }
    if (load) {
        const QString message = QObject::tr("Opened \"%1\", %2x%3, Depth: %4")
            .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
        parent->statusBar()->showMessage(message);
        setImage(image, graphicsArea);
    }
}

void ImageProcessor::save(QMainWindow * parent) {
    QFileDialog dialog(parent, QObject::tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);
    bool save = false;
    QString err_mesg, fileName;
    while (!save && dialog.exec() == QDialog::Accepted) {
        if (dialog.selectedNameFilter().toStdString() == "Windows BMP image (*.bmp *.dib)") {
            dialog.setDefaultSuffix("bmp");
        } else if (dialog.selectedNameFilter().toStdString() == "PNG image (*.png)") {
            dialog.setDefaultSuffix("png");
        } else if (dialog.selectedNameFilter().toStdString() == "JPEG Image (*.jpeg *.jpg *.jpe *.jfif)") {
            dialog.setDefaultSuffix("jpg");
        }
        fileName = dialog.selectedFiles().first();
        QImage to_save = map.toImage();
        save = file_manager.save_image(fileName, err_mesg, to_save);
       if (!save) {
           QMessageBox::information(parent, QGuiApplication::applicationDisplayName(),
                   QObject::tr("Cannot write %1: %2")
                     .arg(QDir::toNativeSeparators(fileName)), err_mesg);
       }
    }
    if (save) {
        const QString message = QObject::tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
        parent->statusBar()->showMessage(message);
    }
}

void ImageProcessor::zoomInOnSelected(QRect selected_area, QGraphicsView * graphicsArea) {
    QPoint a = selected_area.topLeft();
    QPoint b = selected_area.bottomRight();
    if (a.x() > b.x()) {
        int temp_x = a.x();
        a.setX(b.x());
        b.setX(temp_x);
    }
    if (a.y() > b.y()) {
        int temp_y = a.y();
        a.setY(b.y());
        b.setY(temp_y);
    }
    double scroll_factor;
    scroll_factor = ((double)graphicsArea->size().width()) / (b.x() - a.x());
    if (scroll_factor * (b.y() - a.y()) > graphicsArea->size().height()) {
        scroll_factor = ((double)graphicsArea->size().height()) / (b.y() - a.y());
    }
    scaleImage(scroll_factor, graphicsArea);
    graphicsArea->horizontalScrollBar()->setValue(a.x() * scroll_factor);
    graphicsArea->verticalScrollBar()->setValue(a.y() * scroll_factor);
}

void ImageProcessor::resetZoom(QGraphicsView * graphicsArea) {
    scaleFactor = 1;
    scaleImage(1, graphicsArea);
    double scroll_factor;
    scroll_factor = ((double)graphicsArea->size().width()) / map.size().width();
    if (scroll_factor * map.size().height() > graphicsArea->size().height()) {
        scroll_factor = ((double)graphicsArea->size().height()) / map.size().height();
    }
    scaleImage(scroll_factor, graphicsArea);
}
