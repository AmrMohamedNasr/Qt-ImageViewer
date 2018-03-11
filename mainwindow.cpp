#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <iostream>

#define         MAXIMUM_SCALE       15
#define         MINIMUM_SCALE       0.1
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imageProcesser(*(new ImageProcessor())),
    history(&imageProcesser)
{
    imageProcesser.scene.rubberBand = NULL;
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    ui->setupUi(this);
    create_menu();
    graphicsArea = ui->graphicsView;
    imageProcesser.scene.window = this;
    imageProcesser.scene.imageProcessor = &imageProcesser;
    graphicsArea->setScene(&imageProcesser.scene);
    graphicsArea->setBackgroundRole(QPalette::Dark);
    graphicsArea->setVisible(true);
    imageProcesser.scene.selectFlag = false;
    imageProcesser.scene.rotateFlag = false;
    imageProcesser.scene.rotating = false;
}

void MainWindow::create_menu() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    saveAct = fileMenu->addAction(tr("&Save As..."), this, &MainWindow::save);
    saveAct->setEnabled(false);
    saveAct->setShortcut(tr("Ctrl+S"));
    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));
    selectAct = editMenu->addAction(tr("&select..."), this, &MainWindow::select);
    selectAct->setEnabled(false);
    ui->selectButton->setEnabled(false);
    selectAct->setShortcut(tr("Ctrl+L"));
    editMenu->addSeparator();
    cropAct = editMenu->addAction(tr("&crop..."), this, &MainWindow::crop);
    cropAct->setEnabled(false);
    cropAct->setShortcut(tr("Ctrl+C"));
    ui->cropButton->setEnabled(false);
    zoomInSelecAct = editMenu->addAction(tr("&zoom in on area..."), this, &MainWindow::zoomInSelec);
    zoomInSelecAct->setEnabled(false);
    zoomInSelecAct->setShortcut(tr("Ctrl+V"));
    ui->zoomButton->setEnabled(false);
    editMenu->addSeparator();
    rotateAct = editMenu->addAction(tr("&rotate..."), this, &MainWindow::rotate);
    rotateAct->setEnabled(false);
    rotateAct->setShortcut(tr("Ctrl+R"));
    ui->rotateButton->setEnabled(false);
    resetZoomAct = editMenu->addAction(tr("&fit image..."), this, &MainWindow::resetZoom);
    resetZoomAct->setEnabled(false);
    resetZoomAct->setShortcut(tr("Ctrl+F"));
    ui->fitButton->setEnabled(false);
    editMenu->addSeparator();
    zoomInAct = editMenu->addAction(tr("&zoom in..."), this, &MainWindow::zoomIn);
    zoomInAct->setEnabled(false);
    zoomInAct->setShortcut(tr("Ctrl++"));
    ui->zoomInButton->setEnabled(false);
    zoomOutAct = editMenu->addAction(tr("&zoom out..."), this, &MainWindow::zoomOut);
    zoomOutAct->setEnabled(false);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    ui->zoomOutButton->setEnabled(false);
    editMenu->addSeparator();
    undoAct = editMenu->addAction(tr("&undo..."), this, &MainWindow::undo);
    undoAct->setEnabled(false);
    undoAct->setShortcut(tr("Ctrl+Z"));
    redoAct = editMenu->addAction(tr("&redo..."), this, &MainWindow::redo);
    redoAct->setEnabled(false);
    redoAct->setShortcut(tr("Ctrl+Y"));
    resetAct = editMenu->addAction(tr("&reset..."), this, &MainWindow::reset);
    resetAct->setEnabled(false);
    resetAct->setShortcut(tr("Ctrl+`"));
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *helpAct = helpMenu->addAction(tr("&Help..."), this, &MainWindow::help);
    helpAct->setShortcut(tr("Ctrl+H"));
    helpMenu->addAction(tr("&About..."), this, &MainWindow::about);
    ui->moveButton->setEnabled(false);
}

void MainWindow::help() {
    QMessageBox::about(this, tr("Image Editor Help"),
        tr("<p>This is a simple <b>Image processing</b> app, This application supports "
           "many features such as :"
           "<p><b>Open</b>: It can open images of many types for example :“png”,“jpg” and “bmp” </p>"
           "<p><b>Fit to window</b>: This changes the size of the image to fit the window.</p>"
           "<p><b>Crop</b>: It can crop the opened image with any size you want by first selecting the part of image needed to be cropped then you press crop.</p>"
           "<p><b>Zoom</b>: It can zoom in & out the opened image.</p>"
           "<p><b>Rotate</b>: It can rotate the opened image by first pressing rotate then pressing the mouse left button on image and dragging</p> "
           "<p><b>Undo and Redo</b>: It can undo or redo any actions you did on the opened image.</p> "
           "<p><b>Reset</b>: It can reset the opened image to set it back to the original one. you can not undo or redo after doing a reset.</p>"
           "<p><b>Save</b>: It can save the modified image with any type and in directory.</p>"
           "<p><b>Zoom on selected</b>: It can zoom in on a selected area.</p>"
           "<p>You can use these features from the toolbar found on the left of the window or from the menubar.</p>"
           ));
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Image Editor"),
                tr("<p>This app was made by: "
                   "<p>Amr Nasr</p>"
                   "<p>Michael Raafat</p>"
                   "<p>Ahmed Magdy</p>"
                   "<p>Bishoy Nader</p>"
                   "<p>Marc Magdy</p>"
                   ));
}

void MainWindow::undo() {
    disable_mouse_options();
    history.undo(graphicsArea);
    enable_actions(false);
}
void MainWindow::reset() {
    disable_mouse_options();
    history.reset(graphicsArea);
    enable_actions(false);
}
void MainWindow::redo() {
    disable_mouse_options();
    history.redo(graphicsArea);
    enable_actions(false);
}

void  MainWindow::select() {
    imageProcesser.scene.selectFlag = true;
}

void MainWindow::crop() {
    disable_mouse_options();
    command cmd;
    cmd.img = imageProcesser.image;
    cmd.rectangle = new QRect(imageProcesser.scene.myRect.topLeft(), imageProcesser.scene.myRect.bottomRight());
    cmd.type = CROP;
    cmd.factor = imageProcesser.current_degree;
    cmd.factor2 = imageProcesser.scaleFactor;
    cmd.scrollPoints = *(new QPoint(graphicsArea->horizontalScrollBar()->value(),
                                  graphicsArea->verticalScrollBar()->value()));
    history.addHistory(cmd);
    imageProcesser.cropImage(imageProcesser.scene.myRect, graphicsArea);
    if (imageProcesser.scene.rubberBand != NULL) {
        delete imageProcesser.scene.rubberBand;
        imageProcesser.scene.rubberBand = NULL;
    }
    enable_actions(false);
}

void MainWindow::rotate() {
    disable_mouse_options();
    imageProcesser.scene.rotateFlag = true;
    enable_actions(false);
}

void MainWindow::open() {
    disable_mouse_options();
    imageProcesser.open(this, graphicsArea);
    history.clearHistory();
    history.setOrg(imageProcesser.image);
    enable_actions(false);
}

void MainWindow::save() {
    disable_mouse_options();
    imageProcesser.save(this);
    enable_actions(false);
}

void MainWindow::enable_actions(bool select_area) {
    saveAct->setEnabled(true);
    selectAct->setEnabled(true);
    ui->moveButton->setEnabled(true);
    ui->selectButton->setEnabled(true);
    rotateAct->setEnabled(true);
    ui->rotateButton->setEnabled(true);
    resetZoomAct->setEnabled(true);
    ui->fitButton->setEnabled(true);
    zoomInAct->setEnabled(imageProcesser.scaleFactor < MAXIMUM_SCALE);
    ui->zoomInButton->setEnabled(imageProcesser.scaleFactor < MAXIMUM_SCALE);
    zoomOutAct->setEnabled(imageProcesser.scaleFactor > MINIMUM_SCALE);
    ui->zoomOutButton->setEnabled(imageProcesser.scaleFactor > MINIMUM_SCALE);
    cropAct->setEnabled(select_area);
    ui->cropButton->setEnabled(select_area);
    zoomInSelecAct->setEnabled(select_area);
    ui->zoomButton->setEnabled(select_area);
    undoAct->setEnabled(history.hasUndo());
    redoAct->setEnabled(history.hasRedo());
    resetAct->setEnabled(history.hasUndo());
}
void MainWindow::zoomIn() {
    disable_mouse_options();
    command cmd;
    cmd.factor = 1.25;
    cmd.type = SCALE;
    history.addHistory(cmd);
    imageProcesser.scaleImage(1.25, graphicsArea);
    enable_actions(false);
}
void MainWindow::zoomOut() {
    disable_mouse_options();
    command cmd;
    cmd.factor = 0.8;
    cmd.type = SCALE;
    history.addHistory(cmd);
    imageProcesser.scaleImage(0.8, graphicsArea);
    enable_actions(false);
}
void MainWindow::zoomInSelec() {
    disable_mouse_options();
    command cmd;
    cmd.scrollPoints = *(new QPoint(graphicsArea->horizontalScrollBar()->value(),
                                  graphicsArea->verticalScrollBar()->value()));
    cmd.rectangle = new QRect();
    cmd.rectangle->setTopLeft(imageProcesser.scene.myRect.topLeft());
    cmd.rectangle->setBottomRight(imageProcesser.scene.myRect.bottomRight());
    cmd.type = SELECT_ZOOM;
    imageProcesser.zoomInOnSelected(imageProcesser.scene.myRect, graphicsArea);
    cmd.factor = 1 / imageProcesser.scaleFactor;
    history.addHistory(cmd);
    enable_actions(false);
}

void MainWindow::resetZoom() {
    disable_mouse_options();
    command cmd;
    cmd.factor = 1/imageProcesser.scaleFactor;
    cmd.type = SCALE;
    imageProcesser.resetZoom(graphicsArea);
    cmd.factor *= imageProcesser.scaleFactor;
    history.addHistory(cmd);
    enable_actions(false);
}

void MainWindow::disable_mouse_options() {
    if (imageProcesser.scene.rubberBand != NULL) {
        delete imageProcesser.scene.rubberBand;
        imageProcesser.scene.rubberBand = NULL;
    }
    imageProcesser.scene.selectFlag = false;
    imageProcesser.scene.rotateFlag = false;
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_selectButton_clicked()
{
    select();
}

void MainWindow::on_cropButton_clicked()
{
    crop();
}

void MainWindow::on_rotateButton_clicked()
{
    rotate();
}

void MainWindow::on_zoomInButton_clicked()
{
    zoomIn();
}

void MainWindow::on_zoomOutButton_clicked()
{
    zoomOut();
}

void MainWindow::on_zoomButton_clicked()
{
    zoomInSelec();
}

void MainWindow::on_fitButton_clicked()
{
    resetZoom();
}
void MainWindow::disable_fit_zoom() {
    zoomInSelecAct->setEnabled(false);
    ui->zoomButton->setEnabled(false);
}

void MainWindow::on_moveButton_clicked()
{
    disable_mouse_options();
    enable_actions(false);
}
