#include <SchemaProgram.h>

// Реализация конструктора
SchemaProgram::SchemaProgram(const QString& text){
    SchemaTree tree = SchemaTree(text);
    tree.getRoot();
}
