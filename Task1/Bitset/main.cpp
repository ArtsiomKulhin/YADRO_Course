#include <iostream>
#include <cstdint>
#include <utility>
#include <cassert>

class bitset {
private:
    static constexpr size_t BITS_PER_WORD = 64;

    uint64_t* data_;
    size_t capacity_;

    static size_t words_for(size_t bits) {
        return (bits + BITS_PER_WORD - 1) / BITS_PER_WORD;
    }

    void expand(size_t new_cap) {
        size_t new_words = words_for(new_cap);
        size_t old_words = words_for(capacity_);

        uint64_t* new_data = new uint64_t[new_words];

        for (size_t i = 0; i < new_words; ++i) {
            new_data[i] = 0;
        }

        if (data_ != nullptr) {
            for (size_t i = 0; i < old_words; ++i) {
                new_data[i] = data_[i];
            }
        }

        delete[] data_;
        data_ = new_data;
        capacity_ = new_cap;
    }

public:
    bitset() {
        data_ = nullptr;
        capacity_ = 0;
    }

    explicit bitset(size_t initial_cap) {
        capacity_ = initial_cap;
        size_t words = words_for(capacity_);

        if (words > 0) {
            data_ = new uint64_t[words];
            for (size_t i = 0; i < words; ++i) {
                data_[i] = 0;
            }
        } else {
            data_ = nullptr;
        }
    }

    ~bitset() {
        delete[] data_;
    }

    bitset(const bitset& other) {
        capacity_ = other.capacity_;
        size_t words = words_for(capacity_);

        if (words > 0) {
            data_ = new uint64_t[words];
            for (size_t i = 0; i < words; ++i) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
    }

    bitset& operator=(const bitset& other) {
        if (this != &other) {
            bitset temp(other);
            std::swap(data_, temp.data_);
            std::swap(capacity_, temp.capacity_);
        }
        return *this;
    }

    bitset(bitset&& other) noexcept {
        data_ = other.data_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.capacity_ = 0;
    }

    bitset& operator=(bitset&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            capacity_ = other.capacity_;

            other.data_ = nullptr;
            other.capacity_ = 0;
        }
        return *this;
    }

    void set(size_t k, bool b) {
        if (k >= capacity_) {
            if (!b) return;

            size_t new_cap = capacity_;
            if (new_cap == 0) {
                new_cap = BITS_PER_WORD;
            }

            while (new_cap <= k) {
                new_cap *= 2;
            }
            expand(new_cap);
        }

        size_t idx = k / BITS_PER_WORD;
        uint64_t mask = 1LL << (k % BITS_PER_WORD);

        if (b) {
            data_[idx] |= mask;
        } else {
            data_[idx] &= ~mask;
        }
    }

    bool test(size_t k) const {
        if (k >= capacity_) {
            return false;
        }

        size_t idx = k / BITS_PER_WORD;
        uint64_t mask = 1LL << (k % BITS_PER_WORD);

        return (data_[idx] & mask) != 0;
    }

    bool operator[](size_t k) const {
        return test(k);
    }

    bitset union_with(const bitset& other) const {
        size_t max_cap = capacity_;
        if (other.capacity_ > max_cap) {
            max_cap = other.capacity_;
        }

        bitset result(max_cap);

        size_t my_words = words_for(capacity_);
        size_t other_words = words_for(other.capacity_);
        size_t res_words = words_for(max_cap);

        for (size_t i = 0; i < res_words; ++i) {
            uint64_t val1 = 0;
            if (i < my_words) {
                val1 = data_[i];
            }

            uint64_t val2 = 0;
            if (i < other_words) {
                val2 = other.data_[i];
            }

            result.data_[i] = val1 | val2;
        }

        return result;
    }

    bitset intersection(const bitset& other) const {
        size_t min_cap = capacity_;
        if (other.capacity_ < min_cap) {
            min_cap = other.capacity_;
        }

        bitset result(min_cap);
        size_t words = words_for(min_cap);

        for (size_t i = 0; i < words; ++i) {
            result.data_[i] = data_[i] & other.data_[i];
        }

        return result;
    }

    bool is_subset(const bitset& other) const {
        size_t my_words = words_for(capacity_);
        size_t other_words = words_for(other.capacity_);

        size_t min_words = my_words;
        if (other_words < min_words) {
            min_words = other_words;
        }

        for (size_t i = 0; i < min_words; ++i) {
            if ((data_[i] & ~other.data_[i]) != 0) {
                return false;
            }
        }

        for (size_t i = min_words; i < my_words; ++i) {
            if (data_[i] != 0) {
                return false;
            }
        }

        return true;
    }

    size_t size() const {
        return capacity_;
    }

    bool empty() const {
        size_t words = words_for(capacity_);
        for (size_t i = 0; i < words; ++i) {
            if (data_[i] != 0) return false;
        }
        return true;
    }

    void clear() {
        size_t words = words_for(capacity_);
        for (size_t i = 0; i < words; ++i) {
            data_[i] = 0;
        }
    }
};

int main() {
    std::cout << "Good!\n";
    return 0;
}
