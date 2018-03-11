#include "history.h"
#include <iostream>

using namespace std;
History::History(ImageProcessor * processor) {
    imageProcessor = processor;
    undoStack = std::stack<command>();
    redoStack = std::stack<command>();
}

void History::addHistory(command c) {
    redoStack = std::stack<command>();
    undoStack.push(c);
}

void History::undo(QGraphicsView * graphicsArea) {
    if(undoStack.empty())
        return;
    command currentCommand = undoStack.top();
    undoStack.pop();
    switch (currentCommand.type) {
        case ROTATE:
            imageProcessor->rotateImage(-1.0*currentCommand.factor,false);
            break;
        case SCALE:
            imageProcessor->scaleImage(1.0 / currentCommand.factor,graphicsArea);
            break;
        case CROP:
            imageProcessor->setImage(currentCommand.img, graphicsArea);
            imageProcessor->rotateImage(currentCommand.factor, false);
            imageProcessor->scaleImage(currentCommand.factor2, graphicsArea);
            graphicsArea->horizontalScrollBar()->setValue(currentCommand.scrollPoints.x());
            graphicsArea->verticalScrollBar()->setValue(currentCommand.scrollPoints.y());
            break;
        case SELECT_ZOOM:
            imageProcessor->scaleImage(currentCommand.factor, graphicsArea);
            graphicsArea->horizontalScrollBar()->setValue(currentCommand.scrollPoints.x());
            graphicsArea->verticalScrollBar()->setValue(currentCommand.scrollPoints.y());
            break;
    }
    redoStack.push(currentCommand);
}


void History::redo(QGraphicsView * graphicsArea) {
    if(redoStack.empty())
        return;
    command currentCommand = redoStack.top();
    redoStack.pop();
    switch (currentCommand.type) {
        case ROTATE:
            imageProcessor->rotateImage(currentCommand.factor,false);
            break;
        case SCALE:
            imageProcessor->scaleImage(currentCommand.factor,graphicsArea);
            break;
        case CROP:
            imageProcessor->cropImage(*currentCommand.rectangle,graphicsArea);
            break;
        case SELECT_ZOOM:
            imageProcessor->zoomInOnSelected(*currentCommand.rectangle, graphicsArea);
            break;
    }
    undoStack.push(currentCommand);
}

void History::clearHistory() {
    undoStack = std::stack<command>();
    redoStack = std::stack<command>();
}

void History::reset(QGraphicsView * graphicsArea) {
    imageProcessor->setImage(orgImg, graphicsArea);
    clearHistory();
}

bool History::hasUndo() {
    return !undoStack.empty();
}
bool History::hasRedo() {
    return !redoStack.empty();
}

void History::setOrg(QImage img) {
    orgImg = img;
}
