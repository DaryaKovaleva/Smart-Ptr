#ifndef MEMORYSPAN_H
#define MEMORYSPAN_H

#include <cstddef>
#include <stdexcept>
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "MsPtr.h" 

template <typename T>
class MemorySpan {
private:
    T* data;
    size_t size;

public:
    // Конструктор
    MemorySpan(size_t n) : size(n) {
        if (n > 0) {
            data = new T[n]();  // () для инициализации нулями
        } else {
            data = nullptr;
        }
    }

    // Деструктор
    ~MemorySpan() {
        delete[] data;
    }

    // Запрет копирования
    MemorySpan(const MemorySpan&) = delete;
    MemorySpan& operator=(const MemorySpan&) = delete;

    // Проверка границ
    void checkIndex(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range in MemorySpan");
        }
    }

    // Get: возвращает UnqPtr на КОПИЮ объекта
    UnqPtr<T> Get(size_t index) {
        checkIndex(index);
        T* copy = new T(data[index]);
        return UnqPtr<T>(copy);
    }

    // Copy: возвращает ShrdPtr на копию объекта
    ShrdPtr<T> Copy(size_t index) {
        checkIndex(index);
        T* copy = new T(data[index]);
        return ShrdPtr<T>(copy);
    }

        // Locate: возвращает безопасный указатель MsPtr
    MsPtr<T> Locate(size_t index) {
        checkIndex(index);
        return MsPtr<T>(&data[index], data, data + size);
    }

    // Базовые методы
    size_t GetSize() const { return size; }
    bool IsEmpty() const { return size == 0; }
};

#endif