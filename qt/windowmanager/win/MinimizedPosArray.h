#ifndef MINIMIZEDPOSARRAY_H
#define MINIMIZEDPOSARRAY_H

#include <iostream>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <algorithm>

class MinimizedPosArray {
public:
    static MinimizedPosArray& getInstance() {
        static MinimizedPosArray instance;
        return instance;
    }

    int getSmallestAvailable();
    void markPositionAsTaken(int pos);
    void freePosition(int pos);

private:
    MinimizedPosArray();
    std::vector<int> minimizedSlots;
    std::mutex mtx;

    MinimizedPosArray(const MinimizedPosArray&) = delete;
    void operator=(const MinimizedPosArray&) = delete;
};

#endif // MINIMIZEDPOSARRAY_H
