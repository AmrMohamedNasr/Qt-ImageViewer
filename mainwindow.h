#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QAction>
#include "filemanager.h"
#include <QMouseEvent>
#include "imageprocessor.h"
#include "imagescene.h"
#include "history.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QGraphicsView;
class QScrollBar;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow *ui;
    FileManager file_manager;
    ImageProcessor& imageProcesser;
    History history;
    QGraphicsView * graphicsArea;
    qreal current_degree;
    QAction *saveAct;
    QAction *zoomInSelecAct;
    QAction *resetZoomAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *selectAct;
    QAction *cropAct;
    QAction *rotateAct;
    QAction *resetAct;
    QAction *redoAct;
    QAction *undoAct;
    void create_menu();
    void enable_actions(bool select_area);
    void disable_mouse_options();
    void disable_fit_zoom();
    ~MainWindow();
private slots:
    void select();
    void rotate();
    void open();
    void save();
    void zoomInSelec();
    void resetZoom();
    void zoomIn();
    void zoomOut();
    void crop();
    void undo();
    void reset();
    void redo();
    void help();
    void about();
    void on_selectButton_clicked();
    void on_cropButton_clicked();
    void on_rotateButton_clicked();
    void on_zoomInButton_clicked();
    void on_zoomOutButton_clicked();
    void on_zoomButton_clicked();
    void on_fitButton_clicked();
    void on_moveButton_clicked();
};

#endif // MAINWINDOW_H
