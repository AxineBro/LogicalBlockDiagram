#include "NameGenerator.h"
#include <ctime>
#include <QString>

NameGenerator::NameGenerator() : rng(std::time(nullptr)) {}

NameGenerator::NameGenerator(unsigned int seed) : rng(seed) {}

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

QString NameGenerator::generateNumericPrefix() {
    std::uniform_int_distribution<int> numDist(10000, 999999);
    return "n" + QString::number(numDist(rng));
}

QString NameGenerator::generateLetterPrefix() {
    std::uniform_int_distribution<int> numDist(10000, 99999999);
    return "E" + QString::number(numDist(rng));
}

QString NameGenerator::generateLogicSuffix() {
    std::uniform_int_distribution<int> mainNumDist(1000, 9999);
    std::uniform_int_distribution<int> suffixDist(1, 9);

    return "logic" + QString::number(mainNumDist(rng)) +
           "_" + QString::number(suffixDist(rng));
}

int NameGenerator::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
