#ifndef MSPTR_H
#define MSPTR_H
#include <stdexcept>
template <typename T>
class MsPtr {
private:
    T* ptr;
    T* begin;
    T* end;

public:
    // Конструктор: принимает текущий указатель, начало и конец
    MsPtr(T* p, T* b, T* e) : ptr(p), begin(b), end(e) {
    }

    // Оператор разыменования (получить значение)
    T& operator*() const {
        return *ptr;
    }

    // Оператор стрелочки (вызвать метод объекта)
    T* operator->() const {
        return ptr;
    }
    // Оператор сложения: сдвиг на n элементов
    MsPtr operator+(size_t n) const {
        T* new_ptr = ptr + n;
        
        // Проверка границ: новый указатель должен быть >= begin и <= end
        if (new_ptr < begin || new_ptr >= end) {
            throw std::out_of_range("MsPtr arithmetic out of bounds");
        }
        
        // Возвращаем НОВЫЙ MsPtr с проверенным указателем
        return MsPtr(new_ptr, begin, end);
    }

    // Префиксный инкремент: ++ptr
    MsPtr& operator++() {
        T* new_ptr = ptr + 1;
        
        // Проверка границ
        if (new_ptr >= end) {
            throw std::out_of_range("MsPtr increment out of bounds");
        }
        
        ptr = new_ptr;
        return *this;
    }
    MsPtr operator-(size_t n) const {
        T* new_ptr = ptr - n;
        
        // Проверка границ: новый указатель должен быть >= begin и <= end
        if (new_ptr <= begin || new_ptr > end) {
            throw std::out_of_range("MsPtr arithmetic out of bounds");
        }
        
        // Возвращаем НОВЫЙ MsPtr с проверенным указателем
        return MsPtr(new_ptr, begin, end);
    }

    // Префиксный инкремент: --ptr
    MsPtr& operator--() {
        T* new_ptr = ptr - 1;
        
        // Проверка границ
        if (new_ptr <= begin) {
            throw std::out_of_range("MsPtr decrement out of bounds");
        }
        
        ptr = new_ptr;
        return *this;
    }
};

#endif