#include <iostream>
#include <cassert>
#include <ctime>
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "MemorySpan.h"
#include "MsPtr.h"

// --- Вспомогательные классы ---
class Base {
public:
    virtual ~Base() {}
    virtual int getValue() const { return 10; }
};

class Derived : public Base {
public:
    int getValue() const override { return 20; }
};

// --- Функция замера времени ---
double measureTime(void (*func)(int), int iterations) {
    clock_t start = clock();
    func(iterations);
    clock_t end = clock();
    return (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
}

// --- Печать заголовка таблицы с разным N ---
void printTableHeader(const char* title) {
    std::cout << std::endl << "=== " << title << " ===" << std::endl;
    std::cout << "+------------+----------------+----------------+" << std::endl;
    std::cout << "| N          | Raw Ptr (ms)   | Smart Ptr (ms) |" << std::endl;
    std::cout << "+------------+----------------+----------------+" << std::endl;
}

// --- Печать строки таблицы ---
void printTableRow(int n, double rawTime, double smartTime) {
    std::cout << "| " << n;
    // Дополняем до 10 символов
    int tmp = n, len = 0;
    while (tmp > 0) { ++len; tmp /= 10; }
    for (int i = len; i < 10; ++i) std::cout << ' ';
    
    std::cout << " | " << rawTime;
    // Дополняем до 14 символов
    int rawInt = (int)rawTime;
    len = 0; tmp = rawInt > 0 ? rawInt : 1;
    while (tmp > 0) { ++len; tmp /= 10; }
    if (rawTime < 1) len = 3; // "0.XX"
    for (int i = len; i < 14; ++i) std::cout << ' ';
    
    std::cout << " | " << smartTime;
    int smartInt = (int)smartTime;
    len = 0; tmp = smartInt > 0 ? smartInt : 1;
    while (tmp > 0) { ++len; tmp /= 10; }
    if (smartTime < 1) len = 3;
    for (int i = len; i < 14; ++i) std::cout << ' ';
    std::cout << " |" << std::endl;
}

// ============================================================================
// ФУНКЦИОНАЛЬНЫЕ ТЕСТЫ
// ============================================================================

void testUnqPtr() {
    std::cout << "--- UnqPtr Tests ---" << std::endl;
    
    {
        UnqPtr<int> p(new int(42));
        assert(*p == 42);
        assert(p.get() != 0);
        std::cout << "[OK] UnqPtr creation & dereference" << std::endl;
    }
    
    {
        UnqPtr<int> p1(new int(100));
        UnqPtr<int> p2(static_cast<UnqPtr<int>&&>(p1));
        assert(*p2 == 100);
        assert(p1.get() == 0);
        std::cout << "[OK] UnqPtr move semantics" << std::endl;
    }
    
    {
        UnqPtr<int> p(new int(55));
        int* raw = p.release();
        assert(p.get() == 0);
        assert(*raw == 55);
        delete raw;
        std::cout << "[OK] UnqPtr release()" << std::endl;
    }
    
    {
        UnqPtr<int> p(new int(10));
        p.reset(new int(20));
        assert(*p == 20);
        std::cout << "[OK] UnqPtr reset()" << std::endl;
    }
    
    {
        UnqPtr<Derived> d(new Derived());
        UnqPtr<Base> b(static_cast<UnqPtr<Derived>&&>(d));
        assert(b->getValue() == 20);
        assert(d.get() == 0);
        std::cout << "[OK] UnqPtr subtyping" << std::endl;
    }
    
    {
        UnqPtr<int[]> arr(new int[5]);
        arr[0] = 10;
        arr[4] = 50;
        assert(arr[0] == 10);
        assert(arr[4] == 50);
        std::cout << "[OK] UnqPtr array specialization" << std::endl;
    }
}

void testShrdPtr() {
    std::cout << "--- ShrdPtr Tests ---" << std::endl;
    
    {
        ShrdPtr<int> p(new int(5));
        assert(p.use_count() == 1);
        std::cout << "[OK] ShrdPtr creation & use_count" << std::endl;
    }
    
    {
        ShrdPtr<int> p1(new int(7));
        ShrdPtr<int> p2(p1);
        assert(p1.use_count() == 2);
        assert(*p2 == 7);
        std::cout << "[OK] ShrdPtr copy constructor" << std::endl;
    }
    
    {
        ShrdPtr<int> p1(new int(1));
        ShrdPtr<int> p2(new int(2));
        p2 = p1;
        assert(p1.use_count() == 2);
        assert(*p2 == 1);
        std::cout << "[OK] ShrdPtr copy assignment" << std::endl;
    }
    
    {
        ShrdPtr<int> p1(new int(99));
        ShrdPtr<int> p2(static_cast<ShrdPtr<int>&&>(p1));
        assert(*p2 == 99);
        assert(p1.get() == 0);
        std::cout << "[OK] ShrdPtr move semantics" << std::endl;
    }
    
    {
        ShrdPtr<int> p(new int(33));
        int* raw = p.release();
        assert(p.get() == 0);
        assert(*raw == 33);
        delete raw;
        std::cout << "[OK] ShrdPtr release()" << std::endl;
    }
    
    {
        ShrdPtr<int[]> arr(new int[3]);
        arr[0] = 100;
        arr[2] = 300;
        assert(arr[0] == 100);
        assert(arr[2] == 300);
        assert(arr.use_count() == 1);
        std::cout << "[OK] ShrdPtr array specialization" << std::endl;
    }
}

void testMemorySpan() {
    std::cout << "--- MemorySpan Tests ---" << std::endl;
    
    {
        MemorySpan<int> span(5);
        assert(span.GetSize() == 5);
        assert(!span.IsEmpty());
        std::cout << "[OK] MemorySpan creation" << std::endl;
    }
    
    {
        MemorySpan<int> span(3);
        *span.Locate(0) = 42;
        UnqPtr<int> p = span.Get(0);
        assert(*p == 42);
        std::cout << "[OK] MemorySpan Get()" << std::endl;
    }
    
    {
        MemorySpan<int> span(3);
        *span.Locate(1) = 77;
        ShrdPtr<int> p = span.Copy(1);
        assert(*p == 77);
        assert(p.use_count() == 1);
        std::cout << "[OK] MemorySpan Copy()" << std::endl;
    }
    
    {
        MemorySpan<int> span(3);
        bool caught = false;
        try { span.Get(100); } catch (...) { caught = true; }
        assert(caught);
        std::cout << "[OK] MemorySpan bounds check" << std::endl;
    }
}

void testMsPtr() {
    std::cout << "--- MsPtr Tests ---" << std::endl;
    
    {
        MemorySpan<int> span(5);
        for (size_t i = 0; i < span.GetSize(); ++i) {
            *span.Locate(i) = static_cast<int>(i * 10);
        }
        
        MsPtr<int> p = span.Locate(1);
        assert(*p == 10);
        ++p;
        assert(*p == 20);
        p = p + 2;
        assert(*p == 40);
        --p;
        assert(*p == 30);
        p = p - 1;
        assert(*p == 20);
        std::cout << "[OK] MsPtr arithmetic" << std::endl;
    }
    
    {
        MemorySpan<int> span(3);
        MsPtr<int> p = span.Locate(2);
        
        bool caught = false;
        try { ++p; } catch (...) { caught = true; }
        assert(caught);
        
        caught = false;
        try { p = p + 100; } catch (...) { caught = true; }
        assert(caught);
        
        MsPtr<int> p2 = span.Locate(0);
        caught = false;
        try { --p2; } catch (...) { caught = true; }
        assert(caught);
        
        std::cout << "[OK] MsPtr bounds protection" << std::endl;
    }
}

// ============================================================================
// НАГРУЗОЧНЫЕ ТЕСТЫ (6 таблиц с разным N)
// ============================================================================

// --- Бенчмарк-функции ---
void bench_raw_create(int n) {
    for (int i = 0; i < n; ++i) { int* p = new int(i); delete p; }
}
void bench_unq_create(int n) {
    for (int i = 0; i < n; ++i) { UnqPtr<int> p(new int(i)); }
}

void bench_raw_assign(int n) {
    for (int i = 0; i < n; ++i) { int* p1 = new int(1); int* p2 = p1; p1 = 0; delete p2; }
}
void bench_unq_move(int n) {
    for (int i = 0; i < n; ++i) { UnqPtr<int> p1(new int(1)); UnqPtr<int> p2(static_cast<UnqPtr<int>&&>(p1)); }
}

void bench_unq_reset(int n) {
    for (int i = 0; i < n; ++i) {
        UnqPtr<int> p(new int(1));
        for (int j = 0; j < 10; ++j) p.reset(new int(j));
    }
}

void bench_shrd_create(int n) {
    for (int i = 0; i < n; ++i) { ShrdPtr<int> p(new int(i)); }
}

void bench_shrd_copy(int n) {
    for (int i = 0; i < n; ++i) {
        ShrdPtr<int> p1(new int(1));
        for (int j = 0; j < 10; ++j) { ShrdPtr<int> p2(p1); }
    }
}

void testPerformance() {
    std::cout << std::endl << "========================================" << std::endl;
    std::cout << "=== PERFORMANCE BENCHMARKS (different N) ===" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Массив разных значений N для масштабирования
    const int sizes[] = { 100000, 500000, 1000000, 5000000 };
    const int numSizes = 4;
    
    // --- Таблица 1: UnqPtr Creation ---
    printTableHeader("Table 1: UnqPtr Creation");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        double rawTime = measureTime(bench_raw_create, n);
        double smartTime = measureTime(bench_unq_create, n);
        printTableRow(n, rawTime, smartTime);
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
    
    // --- Таблица 2: UnqPtr Move ---
    printTableHeader("Table 2: UnqPtr Move");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        double rawTime = measureTime(bench_raw_assign, n);
        double smartTime = measureTime(bench_unq_move, n);
        printTableRow(n, rawTime, smartTime);
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
    
    // --- Таблица 3: UnqPtr Reset ---
    printTableHeader("Table 3: UnqPtr Reset (10 resets per iteration)");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i] / 10; // меньше итераций, так как внутри цикл на 10
        double smartTime = measureTime(bench_unq_reset, n);
        std::cout << "| " << n;
        int tmp = n, len = 0;
        while (tmp > 0) { ++len; tmp /= 10; }
        for (int j = len; j < 10; ++j) std::cout << ' ';
        std::cout << " | N/A            | " << smartTime;
        int smartInt = (int)smartTime;
        len = 0; tmp = smartInt > 0 ? smartInt : 1;
        while (tmp > 0) { ++len; tmp /= 10; }
        if (smartTime < 1) len = 3;
        for (int j = len; j < 14; ++j) std::cout << ' ';
        std::cout << " |" << std::endl;
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
    
    // --- Таблица 4: ShrdPtr Creation ---
    printTableHeader("Table 4: ShrdPtr Creation");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        double rawTime = measureTime(bench_raw_create, n);
        double smartTime = measureTime(bench_shrd_create, n);
        printTableRow(n, rawTime, smartTime);
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
    
    // --- Таблица 5: ShrdPtr Deletion ---
    printTableHeader("Table 5: ShrdPtr Deletion");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i];
        double rawTime = measureTime(bench_raw_create, n);
        double smartTime = measureTime(bench_shrd_create, n);
        printTableRow(n, rawTime, smartTime);
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
    
    // --- Таблица 6: ShrdPtr Copying ---
    printTableHeader("Table 6: ShrdPtr Copying (10 copies per iteration)");
    for (int i = 0; i < numSizes; ++i) {
        int n = sizes[i] / 10;
        double smartTime = measureTime(bench_shrd_copy, n);
        std::cout << "| " << n;
        int tmp = n, len = 0;
        while (tmp > 0) { ++len; tmp /= 10; }
        for (int j = len; j < 10; ++j) std::cout << ' ';
        std::cout << " | N/A            | " << smartTime;
        int smartInt = (int)smartTime;
        len = 0; tmp = smartInt > 0 ? smartInt : 1;
        while (tmp > 0) { ++len; tmp /= 10; }
        if (smartTime < 1) len = 3;
        for (int j = len; j < 14; ++j) std::cout << ' ';
        std::cout << " |" << std::endl;
    }
    std::cout << "+------------+----------------+----------------+" << std::endl;
}

// --- Главная функция ---
void runAllTests() {
    testUnqPtr();
    std::cout << std::endl;
    testShrdPtr();
    std::cout << std::endl;
    testMemorySpan();
    std::cout << std::endl;
    testMsPtr();
    testPerformance();
}