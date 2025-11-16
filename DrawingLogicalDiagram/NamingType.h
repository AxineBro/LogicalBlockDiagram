#ifndef NAMINGTYPE_H
#define NAMINGTYPE_H

/**
 * @enum NameFormat
 * @brief Форматы имен для генератора имен
 *
 * Определяет возможные форматы генерации имен для обозначения
 * выходов, переменных и логических элементов в схеме.
 */
enum class NameFormat {
    NUMERIC_PREFIX,  ///< Числовой префикс (например, "1:", "2:")
    LETTER_PREFIX,   ///< Буквенный префикс (например, "A:", "B:")
    LOGIC_SUFFIX     ///< Логический суффикс (например, "logic")
};

#endif // NAMINGTYPE_H
