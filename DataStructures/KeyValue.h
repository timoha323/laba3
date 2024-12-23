#ifndef KEYVALUE_H
#define KEYVALUE_H

template <typename K, typename V>
struct KeyValue {
    K key;
    V value;

    KeyValue() : key(), value() {}
    KeyValue(const K& k, const V& v) : key(k), value(v) {}
};

#endif // KEYVALUE_H
