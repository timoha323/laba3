#ifndef TEST_HASHTABLE_H
#define TEST_HASHTABLE_H

#include "DataStructures/HashTable.h"
#include <iostream>
#include <string>
#include <random>

void print_table(const HashTable<int, std::string>& table) {
    auto iterator = table.GetIterator();
    std::cout << "Current HashTable contents:\n";
    while (iterator->MoveNext()) {
        std::cout << "  Key: " << iterator->GetCurrentKey()
                  << ", Value: " << iterator->GetCurrentValue() << "\n";
    }
    std::cout << "--------------------------------\n";
}

void basic_tests() {
    std::cout << "Running basic HashTable tests...\n";

    HashTable<int, std::string> table;

    table.Add(1, "Alice");
    table.Add(2, "Bob");
    table.Add(3, "Charlie");
    table.Add(4, "Diana");
    print_table(table);

    if (table.ContainsKey(2)) {
        table.Update(2, "Bobby");
        std::cout << "After updating key 2:\n";
        print_table(table);
    } else {
        std::cout << "Key 2 not found for update.\n";
    }

    if (table.ContainsKey(3)) {
        table.Remove(3);
        std::cout << "After removing key 3:\n";
        print_table(table);
    } else {
        std::cout << "Key 3 not found for removal.\n";
    }

    try {
        std::cout << "Value at key 4: " << table.Get(4) << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    std::cout << "Contains key 1: " << (table.ContainsKey(1) ? "Yes" : "No") << "\n";
    std::cout << "Contains key 5: " << (table.ContainsKey(5) ? "Yes" : "No") << "\n";

    std::cout << "Basic HashTable tests completed.\n";
}

void performance_testss() {
    std::cout << "Running performance HashTable tests...\n";

    const int NUM_ELEMENTS = 10000;
    HashTable<int, std::string> table;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> key_dist(1, NUM_ELEMENTS * 10);
    std::uniform_int_distribution<> value_dist(0, 25);

    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        int key = key_dist(gen);
        std::string value(1, alphabet[value_dist(gen)]);
        table.Add(key, value);
    }
    std::cout << "Added " << NUM_ELEMENTS << " elements.\n";

    for (int i = 0; i < NUM_ELEMENTS / 2; ++i) {
        int key = key_dist(gen);
        if (table.ContainsKey(key)) {
            table.Update(key, "UpdatedValue");
        }
    }
    std::cout << "Updated approximately " << NUM_ELEMENTS / 2 << " elements.\n";

    for (int i = 0; i < NUM_ELEMENTS / 4; ++i) {
        int key = key_dist(gen);
        if (table.ContainsKey(key)) {
            table.Remove(key);
        }
    }
    std::cout << "Deleted approximately " << NUM_ELEMENTS / 4 << " elements.\n";

    std::cout << "HashTable size after deletions: " << table.GetCount() << "\n";

    std::cout << "Performance HashTable tests completed.\n";
}

// Тесты обработки коллизий
void collision_tests() {
    std::cout << "Running collision HashTable tests...\n";

    HashTable<int, std::string> table;

    // Добавляем элементы, которые будут попадать в одну и ту же ячейку (примеры подобраны вручную)
    table.Add(1, "One");
    table.Add(17, "Seventeen"); // Предполагаем, что 1 и 17 попадают в один бакет
    table.Add(33, "Thirty-Three"); // Аналогично, 1 и 33 могут коллидировать

    // Проверка наличия элементов
    print_table(table);

    // Удаление одного из коллизирующих элементов с проверкой
    if (table.ContainsKey(17)) {
        table.Remove(17);
        std::cout << "After removing key 17:\n";
        print_table(table);
    } else {
        std::cout << "Key 17 not found for removal.\n";
    }

    // Проверка доступности остальных элементов
    try {
        std::cout << "Value at key 1: " << table.Get(1) << "\n";
        std::cout << "Value at key 33: " << table.Get(33) << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    std::cout << "Collision HashTable tests completed.\n";
}

// Обобщённый тестовый интерфейс
void test_hashtable() {
    std::cout << "Starting HashTable tests...\n";

    basic_tests();
    performance_testss();
    collision_tests();

    std::cout << "All HashTable tests completed.\n";
}

#endif //TEST_HASHTABLE_H