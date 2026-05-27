#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <vector>

class ReadWriteLock {
private:
    std::mutex mtx_;
    std::condition_variable cv_read_;
    std::condition_variable cv_write_;

    int readers_;
    int waiting_writers_;
    bool is_writing_;

public:
    ReadWriteLock() {
        readers_ = 0;
        waiting_writers_ = 0;
        is_writing_ = false;
    }

    void reader_lock() {
        std::unique_lock<std::mutex> lock(mtx_);

        while (is_writing_ || waiting_writers_ > 0) {
            cv_read_.wait(lock);
        }

        readers_++;
    }

    void reader_unlock() {
        std::lock_guard<std::mutex> lock(mtx_);

        readers_--;
        if (readers_ == 0) {
            cv_write_.notify_one();
        }
    }

    void writer_lock() {
        std::unique_lock<std::mutex> lock(mtx_);

        waiting_writers_++;

        while (readers_ > 0 || is_writing_) {
            cv_write_.wait(lock);
        }

        waiting_writers_--;
        is_writing_ = true;
    }

    void writer_unlock() {
        std::lock_guard<std::mutex> lock(mtx_);

        is_writing_ = false;

        if (waiting_writers_ > 0) {
            cv_write_.notify_one();
        } else {
            cv_read_.notify_all();
        }
    }
};

class ReadGuard {
private:
    ReadWriteLock& rw_lock_;

public:
    ReadGuard(ReadWriteLock& lock) : rw_lock_(lock) {
        rw_lock_.reader_lock();
    }

    ~ReadGuard() {
        rw_lock_.reader_unlock();
    }
};

class WriteGuard {
private:
    ReadWriteLock& rw_lock_;

public:
    WriteGuard(ReadWriteLock& lock) : rw_lock_(lock) {
        rw_lock_.writer_lock();
    }

    ~WriteGuard() {
        rw_lock_.writer_unlock();
    }
};

int main() {
    return 0;
}
