#include "SchemaProgram.h"
#include "DrawingDiagram.h"

// Конструктор программы построения схемы.
SchemaProgram::SchemaProgram(const QString& text, QGraphicsView* view)
{
    SchemaTree tree(text);
    DrawingDiagram diagram(tree.getRoot(), view);

    QGraphicsScene* scene = diagram.buildScene();
    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
