#ifndef SCHEMATYPES_H
#define SCHEMATYPES_H

/**
 * @enum NodeType
 * @brief Типы узлов в дереве разбора
 *
 * Определяет возможные типы узлов при построении
 * дерева из логического выражения.
 */

enum class NodeType {
    VAR,  ///< Переменная (A, B, C, ...)
    OP,   ///< Бинарный оператор (∧, &, |)
    NOT   ///< Унарное отрицание (!)
};

#endif // SCHEMATYPES_H
