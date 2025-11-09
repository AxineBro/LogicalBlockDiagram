#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <SchemaTree.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_executeButton_clicked()
{
    QString text = ui->inputEdit->text();
    SchemaTree tree = SchemaTree(text);
    tree.printTree();
}

