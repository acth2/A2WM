#include "MinimizedPosArray.h"

MinimizedPosArray::MinimizedPosArray() {
    for (int i = 0; i < 50; ++i) {
        minimizedSlots.push_back(i * 95);
    }
}

int MinimizedPosArray::getSmallestAvailable() {
    std::lock_guard<std::mutex> lock(mtx);

    for (int pos : minimizedSlots) {
        if (pos != -1) {
            return pos;
        }
    }
    throw std::runtime_error("No available positions.");
}

void MinimizedPosArray::markPositionAsTaken(int pos) {
    std::lock_guard<std::mutex> lock(mtx);

    auto it = std::find(minimizedSlots.begin(), minimizedSlots.end(), pos);
    if (it != minimizedSlots.end()) {
        *it = -1;
    } else {
        throw std::invalid_argument("Position not found.");
    }
}

void MinimizedPosArray::freePosition(int pos) {
    std::lock_guard<std::mutex> lock(mtx);

    for (int i = 0; i < minimizedSlots.size(); ++i) {
        if (minimizedSlots[i] == -1 && i * 95 == pos) {
            minimizedSlots[i] = pos;
            return;
        }
    }
    throw std::invalid_argument("Position not found to free.");
}
