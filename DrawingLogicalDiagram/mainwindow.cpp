#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <SchemaTree.h>
#include <SchemaProgram.h>
#include <QFileDialog>
#include <QImage>
#include <QPainter>

// Конструктор главного окна.
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

// Деструктор главного окна.
MainWindow::~MainWindow()
{
    delete ui;
}

// Обработчик нажатия кнопки "Выполнить".
void MainWindow::on_executeButton_clicked()
{
    QString text = ui->inputEdit->text();
    SchemaTree tree = SchemaTree(text);
    tree.printTree();
    SchemaProgram program(text, ui->graphicsView);
}

// Обработчик нажатия кнопки "Сохранить".
void MainWindow::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить схему"), "", tr("Изображения (*.png *.jpg *.bmp)"));
    if (fileName.isEmpty()) {
        return;
    }

    QGraphicsScene* scene = ui->graphicsView->scene();
    if (!scene) {
        return;
    }

    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    scene->render(&painter);
}

