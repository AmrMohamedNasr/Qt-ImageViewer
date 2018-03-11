#ifndef IMAGESCENE_H
#define IMAGESCENE_H
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets>
#include "imageprocessor.h"

class QGraphicsSceneMouseEvent;
class QGraphicsPixmapItem;
class QGraphicsRectItem;
class QRubberBand;
class MainWindow;

class ImageScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ImageScene(QObject* parent);
    bool selectFlag;
    bool rotating;
    bool rotateFlag;
    QRect myRect;
    QGraphicsRectItem * rubberBand;
    QPointF tracker;
    MainWindow * window;
    ImageProcessor * imageProcessor;
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
private:
    bool m_leftMouseButtonPressed           {false};
};

#endif // IMAGESCENE_H
