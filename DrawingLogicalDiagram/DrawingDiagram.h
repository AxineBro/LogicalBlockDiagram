#ifndef DRAWINGDIAGRAM_H
#define DRAWINGDIAGRAM_H

#include <SchemaTree.h>
#include <QObject>

class DrawingDiagram : public QObject{
    Q_OBJECT

public:
    DrawingDiagram(const SchemaTree::Node* tree);
    ~DrawingDiagram();
};

#endif // DRAWINGDIAGRAM_H
