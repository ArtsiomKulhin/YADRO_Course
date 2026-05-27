#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <thread>
#include <chrono>

template <typename T>
class BoundedQueue {
private:
    std::queue<T> queue_;
    size_t capacity_;

    std::mutex mtx_;
    std::condition_variable cv_not_full_;
    std::condition_variable cv_not_empty_;

public:
    explicit BoundedQueue(size_t capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        capacity_ = capacity;
    }

    void push(const T& value) {
        std::unique_lock<std::mutex> lock(mtx_);

        while (queue_.size() >= capacity_) {
            cv_not_full_.wait(lock);
        }

        queue_.push(value);
        cv_not_empty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);

        while (queue_.empty()) {
            cv_not_empty_.wait(lock);
        }

        T val = queue_.front();
        queue_.pop();

        cv_not_full_.notify_one();
        return val;
    }

    bool try_push(const T& value) {
        std::unique_lock<std::mutex> lock(mtx_);

        if (queue_.size() >= capacity_) {
            return false;
        }

        queue_.push(value);
        cv_not_empty_.notify_one();
        return true;
    }

    bool try_pop(T& out_value) {
        std::unique_lock<std::mutex> lock(mtx_);

        if (queue_.empty()) {
            return false;
        }

        out_value = queue_.front();
        queue_.pop();

        cv_not_full_.notify_one();
        return true;
    }
};

int main() {

    return 0;
}
