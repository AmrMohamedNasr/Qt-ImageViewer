#ifndef HISTORY_H
#define HISTORY_H
#include <stack>
#include "imageprocessor.h"

enum {ROTATE = 0, SCALE = 1, CROP = 2, SELECT_ZOOM = 3};
struct command {
    int type;
    qreal factor;
    qreal factor2;
    QImage img;
    QRect *rectangle;
    QPoint scrollPoints;
};

class History {
public:
    History(ImageProcessor * processor);
    void addHistory(command c);
    void undo(QGraphicsView * graphicsArea);
    void redo(QGraphicsView * graphicsArea);
    void clearHistory();
    void reset(QGraphicsView * graphicsArea);
    bool hasUndo();
    bool hasRedo();
    void setOrg(QImage img);
private:
    ImageProcessor *imageProcessor;
    std::stack<command> undoStack;
    std::stack<command> redoStack;
    QImage orgImg;
};
#endif // HISTORY_H
