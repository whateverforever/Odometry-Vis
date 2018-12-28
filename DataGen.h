#pragma once

#include "Vis.h"

class DataGenerator {
public:
    DataGenerator();
    void updateValue();
    friend class Vis;
private:
    int m_someValue;
};

DataGenerator::DataGenerator() {
    m_someValue = 555;
}

void DataGenerator::updateValue() {
    m_someValue += 10;
}