#ifndef DRAWINGDIAGRAM_H
#define DRAWINGDIAGRAM_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "SchemaTree.h"
#include "NameGenerator.h"

class DrawingDiagram : public QObject {
    Q_OBJECT
public:
    DrawingDiagram(const SchemaTree::Node* root, QGraphicsView* view, QObject* parent = nullptr);
    QGraphicsScene* buildScene();
private:
    NameGenerator generator;
    qreal varLevelX;
    const SchemaTree::Node* root;
    QGraphicsView* view;
    qreal sceneWidth;
    qreal sceneHeight;
    void drawDemoElements(QGraphicsScene* scene);
    int countAllNodes(const SchemaTree::Node* node) const;
    int calculateHeight(const SchemaTree::Node* node) const;
    int maxChildDepth(const SchemaTree::Node* node) const;  // Добавлена новая функция для вычисления максимальной глубины среди детей
    void drawNode(QGraphicsScene* scene, const SchemaTree::Node* node, qreal connectX, qreal connectY, qreal allocHeight, qreal coefficient, int treeDepth, int currentLevel, qreal widthFactor); // Исправлена сигнатура без лишнего параметра (убран sibling_max_depth для совместимости)
};

#endif // DRAWINGDIAGRAM_H
