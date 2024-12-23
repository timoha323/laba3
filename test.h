#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <ostream>

void run_tests();
void functional_tests();
void performance_tests();
std::vector<int> read_test_sizes(const std::string& filename);

template <typename DictionaryType, typename KeyType, typename ValueType>
void test_dictionary(const std::string& dictionary_name);


template <typename DictionaryType>
void test_sparse_vector(const std::string& dictionary_name, bool extended = false);


template <typename DictionaryType>
void test_sparse_matrix(const std::string& dictionary_name, bool extended = false);


template<typename Func>
long long measure_time(Func func);

template<typename TDictionary>
void performance_test_vector(int size, const std::string& dict_name, std::ostream& log_stream);

template<typename TDictionary>
void performance_test_matrix(int size, const std::string& dict_name, std::ostream& log_stream);

#endif // TEST_H
