#include "main.h"

#include <stdlib.h>
#include <iostream>
#include <thread>
#include <functional>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include "Vis.h"
#include "DataGen.h"

void updateData(DataGenerator *dataGen) {
    while(true) {
        dataGen->updateValue();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int /* argc */, char ** /* argv */) {
    // Lib code!
    auto visu = new Vis();
    auto dataGenerator = new DataGenerator();

    visu->setDataSource(dataGenerator);
    
    // Simulates the threads from Liu & Yu
    std::thread dataThread(updateData, std::ref(dataGenerator));

    visu->initUI();
    visu->startUI();

    dataThread.join();

    return 0;
}