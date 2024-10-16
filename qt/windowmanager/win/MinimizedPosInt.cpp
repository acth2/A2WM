#include "MinimizedPosInt.h"

int MinimizedPosInt::getValue() {
    std::lock_guard<std::mutex> lock(mtx);
    return *sharedInt;
}

void MinimizedPosInt::setValue(int newValue) {
    std::lock_guard<std::mutex> lock(mtx);
    if (newValue <= 0) {
        throw std::invalid_argument("New value must be a positive integer.");
    }
    *sharedInt = newValue;
}
