#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <iterator>

// Самописный контейнер - динамический массив (аналог vector)
template<typename T>
class DynamicArray {
private:
    T* data;
    size_t capacity;
    size_t size_;

    void resize() {
        size_t newCapacity = (capacity == 0) ? 1 : capacity * 2;
        T* newData = static_cast<T*>(::operator new(newCapacity * sizeof(T)));

        for (size_t i = 0; i < size_; ++i) {
            new (newData + i) T(data[i]);
        }

        for (size_t i = 0; i < size_; ++i) {
            data[i].~T();
        }
        ::operator delete(data);

        data = newData;
        capacity = newCapacity;
    }

public:
    // Итератор для DynamicArray
    class Iterator {
    private:
        T* ptr;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(T* p) : ptr(p) {}

        T& operator*() { return *ptr; }
        const T& operator*() const { return *ptr; }
        T* operator->() { return ptr; }
        const T* operator->() const { return ptr; }

        Iterator& operator++() {
            ++ptr;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++ptr;
            return tmp;
        }

        Iterator& operator--() {
            --ptr;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --ptr;
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const Iterator& other) const {
            return ptr != other.ptr;
        }

        Iterator operator+(int n) const {
            return Iterator(ptr + n);
        }

        Iterator operator-(int n) const {
            return Iterator(ptr - n);
        }

        friend Iterator operator+(int n, const Iterator& it) {
            return Iterator(it.ptr + n);
        }

        friend int operator-(const Iterator& a, const Iterator& b) {
            return static_cast<int>(a.ptr - b.ptr);
        }

        bool operator<(const Iterator& other) const {
            return ptr < other.ptr;
        }

        bool operator>(const Iterator& other) const {
            return ptr > other.ptr;
        }

        bool operator<=(const Iterator& other) const {
            return ptr <= other.ptr;
        }

        bool operator>=(const Iterator& other) const {
            return ptr >= other.ptr;
        }

        Iterator& operator+=(int n) {
            ptr += n;
            return *this;
        }

        Iterator& operator-=(int n) {
            ptr -= n;
            return *this;
        }

        T& operator[](int n) {
            return ptr[n];
        }

        const T& operator[](int n) const {
            return ptr[n];
        }
    };

    DynamicArray();
    explicit DynamicArray(size_t initialSize);
    DynamicArray(const DynamicArray& other);
    DynamicArray& operator=(const DynamicArray& other);
    ~DynamicArray();

    void push_back(const T& value);
    void push_back(T&& value);
    void erase(const Iterator& it);
    void erase(const Iterator& first, const Iterator& last);

    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    T& front();
    const T& front() const;
    T& back();
    const T& back() const;

    size_t size() const;
    bool empty() const;
    void clear();

    Iterator begin();
    Iterator end();
    Iterator begin() const;
    Iterator end() const;
};

// Реализация шаблонного класса
#include "dynamic_array.tpp"

#endif // DYNAMIC_ARRAY_H

