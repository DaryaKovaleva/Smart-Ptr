#ifndef SHRDPTR_H
#define SHRDPTR_H

#include <cstddef>      // для size_t
#include <type_traits>  // для std::is_base_of

// --- Базовая версия для одиночных объектов T ---
template <typename T>
class ShrdPtr {
private:
    T* ptr;
    size_t* ref_count;

public:
    // Конструктор
    explicit ShrdPtr(T* p = nullptr) : ptr(p), ref_count(nullptr) {
        if (p != nullptr) {
            ref_count = new size_t(1);
        }
    }

    // Деструктор
    ~ShrdPtr() {
        if (ref_count != nullptr) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }

    // Конструктор копирования
    ShrdPtr(const ShrdPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count != nullptr) {
            (*ref_count)++;
        }
    }

    // Оператор присваивания копированием
    ShrdPtr& operator=(const ShrdPtr& other) {
        if (this != &other) {
            // Освобождаем старое
            if (ref_count != nullptr) {
                (*ref_count)--;
                if (*ref_count == 0) {
                    delete ptr;
                    delete ref_count;
                }
            }
            // Копируем новое
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count != nullptr) {
                (*ref_count)++;
            }
        }
        return *this;
    }

    // Конструктор перемещения
    ShrdPtr(ShrdPtr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    // Оператор присваивания через перемещение
    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) {
            if (ref_count != nullptr) {
                (*ref_count)--;
                if (*ref_count == 0) {
                    delete ptr;
                    delete ref_count;
                }
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    // ПОДТИПИЗАЦИЯ: Шаблонный конструктор копирования от производного типа U
    // Мы просто копируем поля, так как указатели совместимы, а счетчик общий
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    ShrdPtr(const ShrdPtr<U>& other) : ptr(other.get()), ref_count(nullptr) {
        ptr = other.ptr;
        ref_count = other.ref_count;
        if (ref_count != nullptr) {
            (*ref_count)++;
        }
    }

    // Доступ к данным
    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
    T* get() const { return ptr; }

    size_t use_count() const {
        return ref_count ? *ref_count : 0;
    }

    // Сброс
    void reset(T* p = nullptr) {
        if (ref_count != nullptr) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
        ptr = p;
        if (p != nullptr) {
            ref_count = new size_t(1);
        } else {
            ref_count = nullptr;
        }
    }

    // Освобождение владения (release)
    T* release() {
    T* tmp = ptr;
    if (ref_count != nullptr) {
        (*ref_count)--;
        if (*ref_count == 0) {
        }
    }
    ptr = nullptr;
    ref_count = nullptr;
    return tmp;
    }

    explicit operator bool() const { return ptr != nullptr; }
};

// --- Специализация для массивов T[] ---
template <typename T>
class ShrdPtr<T[]> {
private:
    T* ptr;
    size_t* ref_count;

public:
    explicit ShrdPtr(T* p = nullptr) : ptr(p), ref_count(nullptr) {
        if (p != nullptr) {
            ref_count = new size_t(1);
        }
    }

    ~ShrdPtr() {
        if (ref_count != nullptr) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete[] ptr;
                delete ref_count;
            }
        }
    }

    ShrdPtr(const ShrdPtr& other) : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count != nullptr) {
            (*ref_count)++;
        }
    }

    ShrdPtr& operator=(const ShrdPtr& other) {
        if (this != &other) {
            if (ref_count != nullptr) {
                (*ref_count)--;
                if (*ref_count == 0) {
                    delete[] ptr;
                    delete ref_count;
                }
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count != nullptr) {
                (*ref_count)++;
            }
        }
        return *this;
    }

    ShrdPtr(ShrdPtr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    ShrdPtr& operator=(ShrdPtr&& other) noexcept {
        if (this != &other) {
            if (ref_count != nullptr) {
                (*ref_count)--;
                if (*ref_count == 0) {
                    delete[] ptr;
                    delete ref_count;
                }
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    T& operator[](size_t index) const {
        return ptr[index];
    }

    T* get() const { return ptr; }

    size_t use_count() const {
        return ref_count ? *ref_count : 0;
    }

    void reset(T* p = nullptr) {
        if (ref_count != nullptr) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete[] ptr;
                delete ref_count;
            }
        }
        ptr = p;
        if (p != nullptr) {
            ref_count = new size_t(1);
        } else {
            ref_count = nullptr;
        }
    }

    T* release() {
    T* tmp = ptr;
    if (ref_count != nullptr) {
        (*ref_count)--;
        if (*ref_count == 0) {
            delete ref_count;
        }
    }
    ptr = nullptr;
    ref_count = nullptr;
    return tmp;
    }

    explicit operator bool() const { return ptr != nullptr; }
};

#endif