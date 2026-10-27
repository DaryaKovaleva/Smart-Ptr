#ifndef UNQPTR_H
#define UNQPTR_H

#include <cstddef>      // для size_t
#include <type_traits>  // для std::is_base_of (подтипизация)

// --- Базовая версия для одиночных объектов T ---
template <typename T>
class UnqPtr {
private:
    T* ptr;

public:
    // 1. Конструктор
    explicit UnqPtr(T* p = nullptr) : ptr(p) {}

    // 2. Деструктор
    ~UnqPtr() {
        delete ptr;
    }

    // 3. Запрет копирования
    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    // 4. Конструктор перемещения
    UnqPtr(UnqPtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    // 5. Оператор присваивания через перемещение
    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // 6. ПОДТИПИЗАЦИЯ: Конструктор перемещения от производного типа U
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    UnqPtr(UnqPtr<U>&& other) noexcept : ptr(other.release()) {}

    // 7. Доступ к данным
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    // 8. Управление памятью
    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }
};

// --- Специализация для массивов T[] ---
template <typename T>
class UnqPtr<T[]> {
private:
    T* ptr;

public:
    explicit UnqPtr(T* p = nullptr) : ptr(p) {}

    ~UnqPtr() {
        delete[] ptr;
    }

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            delete[] ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

/*
void F (unic_pointer) {}
unic_pointer a
F (std::move (a))
F (a)

*/

    // ПОДТИПИЗАЦИЯ для массивов
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    UnqPtr(UnqPtr<U[]>&& other) noexcept : ptr(other.release()) {}

    T& operator[](size_t index) const { 
        return ptr[index]; 
    }

    T* get() const { return ptr; }

    T* release() {
        T* tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete[] ptr;
            ptr = p;
        }
    }
};

#endif