#include <cassert>
#include <iterator>
#include <quiz/base.h>
#include <string.h>

struct string {
private:
    union metadata {
        struct short_type {
            uint8_t size;
            char data[23];
        } __short;
        struct long_type {
            uint64_t capacity;
            uint64_t size;
            char *data;
        } __long;
    } mdata;

    bool use_short() const {
        return mdata.__short.size & 1;
    }

    void set_short() {
        mdata.__short.size |= 1;
    }

    void unset_short() {
        mdata.__short.size &= 254;
    }

    uint64_t get_capacity() const {
        return ((mdata.__long.capacity >> 1) << 1);
    }

    void set_short_size(size_t len) {
        assert(len <= 23);
        mdata.__short.size = (len << 1) + 1;
    }

    size_t get_short_size() const {
        return mdata.__short.size >> 1;
    }

public:
    string(const char* str) {
        const int n = strlen(str);
        if (n <= 23) [[likely]] {
            set_short();
            assert(use_short());
            memcpy(mdata.__short.data, str, n);
            set_short_size(n); // last bit reserved for use_short()
        } else {
            unset_short();
            assert(!use_short());
            mdata.__long.capacity = n * 2;
            mdata.__long.size = n;
            mdata.__long.data = new char[mdata.__long.capacity];
        }
    }

    string(const string& rhs) = delete;
    string& operator=(const string& rhs) = delete;
    string(string&& rhs) = delete;
    string& operator=(string&& rhs) = delete;

    char& operator[](int idx) {
        if (use_short()) return mdata.__short.data[idx];
        else return mdata.__long.data[idx];
    }

    string& operator+=(const string& rhs) {
        size_t new_size = size() + rhs.size();
        if (new_size <= 23) {
            memcpy(mdata.__short.data + size(),
                    rhs.mdata.__short.data, rhs.size());
            set_short_size(new_size);
        } else {
            // TODO
            assert(0);
        }
        return *this;
    }

    size_t size() const {
        if (use_short()) return get_short_size();
        else return mdata.__long.size;
    }
};

int main() {
    string a = "abc";
    string b = "def";
    a += b;
    for (size_t i = 0; i < a.size(); i ++ ) std::cout << a[i];
    return 0;
}
