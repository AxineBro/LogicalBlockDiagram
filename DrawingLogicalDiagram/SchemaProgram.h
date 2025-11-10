#ifndef SCHEMAPROGRAM_H
#define SCHEMAPROGRAM_H

#include <QObject>
#include <SchemaTree.h>

/**
 * @class ShemaProgram
 * @brief Программа для построения схемы из выражения
 *
 * Класс получает из строкового логического выражения дерево разбора,
 * отрисовыват дерево.
 */
class SchemaProgram : public QObject{
    Q_OBJECT

public:
    /**
     * @brief Конструктор программы построения схемы
     * @param text Логическое выражение в инфиксной нотации
     *
     * Поддерживаемые операторы:
     * - & (AND), | (OR), ^ (XOR), ! (NOT)
     *
     * Пример:
     * @code
     * ShemaProgram program("!((A|B)&!C)");
     * @endcode
     */
    explicit SchemaProgram(const QString& text);

    ~SchemaProgram() = default;

private:
    std::unique_ptr<SchemaTree::Node> root;  ///< Корень дерева
};

#endif // SCHEMAPROGRAM_H
