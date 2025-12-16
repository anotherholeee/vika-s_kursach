#ifndef DYNAMIC_ARRAY_TPP
#define DYNAMIC_ARRAY_TPP

template<typename T>
DynamicArray<T>::DynamicArray() : data(nullptr), capacity(0), size_(0) {}

template<typename T>
DynamicArray<T>::DynamicArray(size_t initialSize) : capacity(initialSize), size_(0) {
    data = static_cast<T*>(::operator new(capacity * sizeof(T)));
}

template<typename T>
DynamicArray<T>::DynamicArray(const DynamicArray& other) : capacity(other.capacity), size_(other.size_) {
    data = static_cast<T*>(::operator new(capacity * sizeof(T)));
    for (size_t i = 0; i < size_; ++i) {
        new (data + i) T(other.data[i]);
    }
}

template<typename T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& other) {
    if (this != &other) {
        for (size_t i = 0; i < size_; ++i) {
            data[i].~T();
        }
        ::operator delete(data);

        capacity = other.capacity;
        size_ = other.size_;
        data = static_cast<T*>(::operator new(capacity * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) {
            new (data + i) T(other.data[i]);
        }
    }
    return *this;
}

template<typename T>
DynamicArray<T>::~DynamicArray() {
    for (size_t i = 0; i < size_; ++i) {
        data[i].~T();
    }
    ::operator delete(data);
}

template<typename T>
void DynamicArray<T>::push_back(const T& value) {
    if (size_ >= capacity) {
        resize();
    }
    new (data + size_) T(value);
    ++size_;
}

template<typename T>
void DynamicArray<T>::push_back(T&& value) {
    if (size_ >= capacity) {
        resize();
    }
    new (data + size_) T(std::move(value));
    ++size_;
}

template<typename T>
void DynamicArray<T>::erase(const Iterator& it) {
    size_t index = it - begin();
    if (index < size_) {
        // Уничтожаем элемент, который удаляем
        data[index].~T();
        // Сдвигаем остальные элементы
        for (size_t i = index; i < size_ - 1; ++i) {
            new (data + i) T(std::move(data[i + 1]));
            data[i + 1].~T();
        }
        --size_;
    }
}

template<typename T>
void DynamicArray<T>::erase(const Iterator& first, const Iterator& last) {
    size_t startIndex = first - begin();
    size_t endIndex = last - begin();
    size_t count = endIndex - startIndex;

    if (startIndex < size_ && endIndex <= size_ && count > 0) {
        // Уничтожаем удаляемые элементы
        for (size_t i = startIndex; i < endIndex; ++i) {
            data[i].~T();
        }
        // Сдвигаем оставшиеся элементы
        for (size_t i = startIndex; i < size_ - count; ++i) {
            new (data + i) T(std::move(data[i + count]));
            data[i + count].~T();
        }
        size_ -= count;
    }
}

template<typename T>
T& DynamicArray<T>::operator[](size_t index) {
    return data[index];
}

template<typename T>
const T& DynamicArray<T>::operator[](size_t index) const {
    return data[index];
}

template<typename T>
T& DynamicArray<T>::front() {
    return data[0];
}

template<typename T>
const T& DynamicArray<T>::front() const {
    return data[0];
}

template<typename T>
T& DynamicArray<T>::back() {
    return data[size_ - 1];
}

template<typename T>
const T& DynamicArray<T>::back() const {
    return data[size_ - 1];
}

template<typename T>
size_t DynamicArray<T>::size() const {
    return size_;
}

template<typename T>
bool DynamicArray<T>::empty() const {
    return size_ == 0;
}

template<typename T>
void DynamicArray<T>::clear() {
    // Уничтожаем все элементы
    for (size_t i = 0; i < size_; ++i) {
        data[i].~T();
    }
    size_ = 0;
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() {
    return Iterator(data);
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() {
    return Iterator(data + size_);
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::begin() const {
    return Iterator(data);
}

template<typename T>
typename DynamicArray<T>::Iterator DynamicArray<T>::end() const {
    return Iterator(data + size_);
}

#endif // DYNAMIC_ARRAY_TPP

