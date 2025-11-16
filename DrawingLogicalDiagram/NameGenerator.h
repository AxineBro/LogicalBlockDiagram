#ifndef NAMEGENERATOR_H
#define NAMEGENERATOR_H

#include <random>
#include <ctime>
#include <QString>
#include "NamingType.h"

class NameGenerator {
private:
    std::mt19937 rng;

public:
    NameGenerator();

    NameGenerator(unsigned int seed);

    QString generateName(NameFormat format);

private:
    QString generateNumericPrefix();

    QString generateLetterPrefix();

    QString generateLogicSuffix();

    int randomInt(int min, int max);
};


#endif // NAMEGENERATOR_H
