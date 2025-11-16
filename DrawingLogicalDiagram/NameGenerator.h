#ifndef NAMEGENERATOR_H
#define NAMEGENERATOR_H

#include <random>
#include <ctime>
#include <QString>
#include "NamingType.h"

/**
 * @class NameGenerator
 * @brief Генератор случайных имён для обозначений в схемах
 *
 * Класс предоставляет функциональность для генерации уникальных имён
 * в различных форматах, используемых в графических схемах.
 * Использует Mersenne Twister для генерации псевдослучайных чисел.
 *
 * @details
 * - Инициализируется текущим временем или заданным seed для воспроизводимости.
 * - Поддерживает три формата имён через NameFormat.
 * - Генерирует имена на основе случайных чисел в заданных диапазонах.
 */
class NameGenerator {
private:
    std::mt19937 rng;   ///< Генератор случайных чисел (Mersenne Twister)

public:

    /**
     * @brief Конструктор по умолчанию
     *
     * Инициализирует генератор случайных чисел текущим временем.
     */
    NameGenerator();

    /**
     * @brief Конструктор с заданным seed
     * @param seed Начальное значение для генератора случайных чисел
     *
     * Позволяет воспроизводить последовательность имён для тестирования.
     */
    NameGenerator(unsigned int seed);

    /**
     * @brief Сгенерировать имя в указанном формате
     * @param format Формат генерируемого имени
     * @return Сгенерированное имя как QString
     *
     * Выбирает соответствующий метод генерации в зависимости от формата.
     * Если формат неизвестен, возвращает имя в формате NUMERIC_PREFIX.
     */
    QString generateName(NameFormat format);

private:

    /**
     * @brief Сгенерировать имя с буквенным префиксом в маленьком регистре
     * @return Имя вида "nXXXXXX" где XXXXXX - случайное число от 10000 до 999999
     */
    QString generateNumericPrefix();

    /**
     * @brief Сгенерировать имя с буквенным префиксом в большом регистре
     * @return Имя вида "EXXXXXXXX" где XXXXXXXX - случайное число от 10000 до 99999999
     */
    QString generateLetterPrefix();

    /**
     * @brief Сгенерировать имя с логическим суффиксом
     * @return Имя вида "logicXXXX_Y" где XXXX - от 1000 до 9999, Y - от 1 до 9
     */
    QString generateLogicSuffix();

    /**
     * @brief Сгенерировать случайное целое в диапазоне
     * @param min Минимальное значение (включительно)
     * @param max Максимальное значение (включительно)
     * @return Случайное число в [min, max]
     */
    int randomInt(int min, int max);
};


#endif // NAMEGENERATOR_H
