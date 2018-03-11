#include "imagescene.h"
#include <QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsRectItem>
#include "mainwindow.h"
#include <iostream>

#define         MAXIMUM_SCALE       15
#define         MINIMUM_SCALE       0.1
#define         MOMENT_MOVE         7
using namespace std;

ImageScene::ImageScene(QObject * parent) : QGraphicsScene(parent)
{
    myRect = *(new QRect());
}

void ImageScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = true;
        if(selectFlag){
            tracker = event->scenePos();
            if (rubberBand != NULL) {
                delete rubberBand;
            }
            // Create a selection square
            rubberBand = new QGraphicsRectItem();
            rubberBand->setBrush(QBrush(QColor(158,182,255,96)));
            rubberBand->setPen(QPen(QColor(158,182,255,200),1));
            // Add it to the graphic scene
            addItem(rubberBand);
        } else if (rotateFlag){
            tracker = event->scenePos();
            rotating = true;
        } else {
            tracker = window->graphicsArea->mapFromScene(event->scenePos());
        }
    }
    QGraphicsScene::mousePressEvent(event);
}

void ImageScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_leftMouseButtonPressed) {
        if (selectFlag) {
            // Form the selection area when moving with the mouse while pressing the LMB
            auto dx = event->scenePos().x() - tracker.x();
            auto dy = event->scenePos().y() - tracker.y();
            int x = tracker.x();
            int y = tracker.y();
            if (dx < 0) {
                x += dx;
                dx = -dx;
            }
            if (dy < 0) {
                y += dy;
                dy = -dy;
            }
            rubberBand->setRect(x, y, dx, dy);
        } else if (rotating) {
            QPointF origin = window->imageProcesser.pixmapItem->mapFromScene(tracker);
            QPointF end = window->imageProcesser.pixmapItem->mapFromScene(event->scenePos());
            QPointF center = window->imageProcesser.pixmapItem->boundingRect().center();
            qreal center_degree = qAtan2(origin.x() - center.x() , origin.y() - center.y()) * 180 * 7 / 22;
            qreal last_degree = qAtan2(end.y() - center.y() , end.x() - center.x()) * 180 * 7 / 22;
            window->current_degree = -90 + center_degree + last_degree;
            window->imageProcesser.rotateImage(window->current_degree, true);
        } else {
            int x_scrl = 0;
            int y_scrl = 0;
            bool upX = true, upY = true;
            QPointF temp = window->graphicsArea->mapFromScene(event->scenePos());
            if (temp.toPoint().x() > tracker.toPoint().x()) {
                x_scrl = window->graphicsArea->horizontalScrollBar()->value() - MOMENT_MOVE;
            } else if (temp.toPoint().x() < tracker.toPoint().x()){
                x_scrl = window->graphicsArea->horizontalScrollBar()->value() + MOMENT_MOVE;
            } else {
                x_scrl = window->graphicsArea->horizontalScrollBar()->value();
                upX = false;
            }
            if (temp.toPoint().y() > tracker.toPoint().y()) {
                y_scrl = window->graphicsArea->verticalScrollBar()->value() - MOMENT_MOVE;
            } else if (temp.toPoint().y() < tracker.toPoint().y()){
                y_scrl = window->graphicsArea->verticalScrollBar()->value() + MOMENT_MOVE;
            } else {
                y_scrl = window->graphicsArea->verticalScrollBar()->value();
                upY = false;
            }
            if (upX) {
                tracker.setX(temp.x());
            }
            if (upY) {
                tracker.setY(temp.y());
            }
            window->graphicsArea->verticalScrollBar()->setValue(y_scrl);
            window->graphicsArea->horizontalScrollBar()->setValue(x_scrl);
        }
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void ImageScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() & Qt::LeftButton) {
        m_leftMouseButtonPressed = false;
        if (selectFlag) {
            QPointF top_left = tracker;
            QPointF bottom_right = event->scenePos();
            if (top_left.x() > bottom_right.x()) {
                int temp_x = top_left.x();
                top_left.setX(bottom_right.x());
                bottom_right.setX(temp_x);
            }
            if (top_left.y() > bottom_right.y()) {
                int temp_y = top_left.y();
                top_left.setY(bottom_right.y());
                bottom_right.setY(temp_y);
            }
            myRect.setTopLeft(imageProcessor->pixmapItem->mapFromScene(top_left).toPoint());
            myRect.setBottomRight(imageProcessor->pixmapItem->mapFromScene(bottom_right).toPoint());
            window->enable_actions(true);
            int possible_factor;
            if (bottom_right.x() == top_left.x()) {
                if (bottom_right.y() == top_left.y()) {
                    window->enable_actions(false);
                } else {
                    possible_factor = window->graphicsArea->size().height() / (bottom_right.y() - top_left.y());
                }
            } else {
                possible_factor = window->graphicsArea->size().width() / (bottom_right.x() - top_left.x());
                if (possible_factor * (bottom_right.y() - top_left.y()) > window->graphicsArea->size().height()) {
                    possible_factor = window->graphicsArea->size().height() / (bottom_right.y() - top_left.y());
                }
            }
            if (window->imageProcesser.scaleFactor * possible_factor > MAXIMUM_SCALE) {
                window->disable_fit_zoom();
            }
        } else if (rotating) {
            command cmd;
            cmd.type = ROTATE;
            cmd.factor = window->current_degree;
            window->history.addHistory(cmd);
            window->imageProcesser.rotateImage(window->current_degree, false);
            window->current_degree = 0;
            rotating = false;
            window->enable_actions(false);
        }
    }
    QGraphicsScene::mouseReleaseEvent(event);
}
