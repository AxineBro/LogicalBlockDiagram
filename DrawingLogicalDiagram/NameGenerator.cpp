#include "NameGenerator.h"
#include <ctime>
#include <QString>

// Конструктор по умолчанию.
NameGenerator::NameGenerator() : rng(std::time(nullptr)) {}

// Конструктор с заданным seed.
NameGenerator::NameGenerator(unsigned int seed) : rng(seed) {}

// Сгенерировать имя в указанном формате.
QString NameGenerator::generateName(NameFormat format) {
    switch(format) {
    case NameFormat::NUMERIC_PREFIX:
        return generateNumericPrefix();
    case NameFormat::LETTER_PREFIX:
        return generateLetterPrefix();
    case NameFormat::LOGIC_SUFFIX:
        return generateLogicSuffix();
    default:
        return generateNumericPrefix();
    }
}

// Сгенерировать имя с буквенным префиксом в маленьком регистре.
QString NameGenerator::generateNumericPrefix() {
    std::uniform_int_distribution<int> numDist(10000, 999999);
    return "n" + QString::number(numDist(rng));
}

// Сгенерировать имя с буквенным префиксом в большом регистре.
QString NameGenerator::generateLetterPrefix() {
    std::uniform_int_distribution<int> numDist(10000, 99999999);
    return "E" + QString::number(numDist(rng));
}

// Сгенерировать имя с логическим суффиксом
QString NameGenerator::generateLogicSuffix() {
    std::uniform_int_distribution<int> mainNumDist(1000, 9999);
    std::uniform_int_distribution<int> suffixDist(1, 9);

    return "logic" + QString::number(mainNumDist(rng)) +
           "_" + QString::number(suffixDist(rng));
}

// Сгенерировать случайное целое в диапазоне.
int NameGenerator::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
