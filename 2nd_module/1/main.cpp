#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

// Задача 1.1 Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы с открытой адресацией.
// Хранимые строки непустые и состоят из строчных латинских букв. 
// Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера. 
// Начальный размер таблицы должен быть равным 8-ми.
// Перехеширование выполняйте при добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4. 
// Структура данных должна поддерживать операции добавления строки в множество,
// удаления строки из множества и проверки принадлежности данной строки множеству. 

// Для разрешения коллизий используйте квадратичное пробирование. i-ая проба 
// g(k, i)=g(k, i-1) + i (mod m). m - степень двойки. 

 
template<typename TYPE>
struct SetNode {
    TYPE key;
    bool used = false;
    bool deleted = false;
};

template<typename TYPE, typename HASHER>
class Set {
private:
    HASHER _hash;
    SetNode<TYPE> *_table;
    std::size_t _capacity;
    std::size_t _filled;

private:
    std::size_t _probe(std::size_t hash, std::size_t i, std::size_t cap) const {
        return (hash + (i * (i + 1)) / 2) & (cap - 1);
    }

    void _tryResizeTable() {
        if ((_filled + 1) * 4 < _capacity * 3) {
            return;
        }

        std::size_t newCap = _capacity << 1;
        SetNode<TYPE> *newTable = new SetNode<TYPE>[newCap];

        for (std::size_t i = 0; i < _capacity; i++) {
            if (_table[i].used && !_table[i].deleted) {
                _insertKey(_table[i].key, newTable, newCap);
            }
        }

        delete[] _table;
        _table = newTable;
        _capacity = newCap;
    }

    void _insertKey(const TYPE& key, SetNode<TYPE> *table, std::size_t cap) {
        std::size_t hash = _hash(key);
        std::size_t firstDeleted = -1;

        for (std::size_t i = 0; i < cap; i++) {
            std::size_t idx = _probe(hash, i, cap);
            auto& cell = table[idx];

            if (!cell.used) {
                std::size_t writeIdx = (firstDeleted != -1) ? firstDeleted : idx;
                table[writeIdx].key = key;
                table[writeIdx].used = true;
                table[writeIdx].deleted = false;
                ++_filled;
                return;
            }

            if (cell.deleted) {
                if (firstDeleted == -1) {
                    firstDeleted = (int)idx;
                }
            }
            else if (cell.key == key) {
                return;
            }
        }

        if (firstDeleted != -1) {
            table[firstDeleted].key = key;
            table[firstDeleted].used = true;
            table[firstDeleted].deleted = false;
            _filled++;
        }
    }

public:
    Set() : _capacity(8), _filled(0), _hash() {
        _table = new SetNode<TYPE>[_capacity];
    }

    ~Set() {
        delete[] _table;
    }

public:
    bool add(const TYPE& key) {
        if (exist(key)) {
            return false;
        }

        _tryResizeTable();
        _insertKey(key, _table, _capacity);
        return true;
    }

    bool remove(const TYPE& key) {
        std::size_t hash = _hash(key);

        for (std::size_t i = 0; i < _capacity; i++) {
            std::size_t idx = _probe(hash, i, _capacity);
            auto& cell = _table[idx];

            if (!cell.used)
                return false;

            if (!cell.deleted && cell.key == key) {
                cell.deleted = true;
                --_filled;
                return true;
            }
        }

        return false;
    }

    bool exist(const TYPE& key) const {
        std::size_t hash = _hash(key);

        for (std::size_t i = 0; i < _capacity; i++) {
            std::size_t idx = _probe(hash, i, _capacity);
            const auto& cell = _table[idx];

            if (!cell.used) {
                return false;
            }

            if (!cell.deleted && cell.key == key) {
                return true;
            }
        }

        return false;
    }
};
 
struct HornerHashing { 
private: 
    std::size_t _p; 

public:
    HornerHashing(std::size_t p = 31) : _p(p) {} 

    std::size_t operator()(const std::string& key) const { 
        std::size_t hash = 0; 
        for (const char &c : key) { 
            hash = hash * _p + (c - 'a' + 1); 
        }

        return hash; 
    } 
}; 

int main() {
    std::unordered_map<char, void (*)(
        Set<std::string, HornerHashing> &set, 
        const std::string &str
    )> commands;
    
    commands['+'] = []( 
        Set<std::string, HornerHashing> &set,
        const std::string &str
    ) {
        if (set.add(str)) {
            std::cout << "OK" << std::endl;
        }
        else {
            std::cout << "FAIL" << std::endl;
        }
    };

    commands['-'] = []( 
        Set<std::string, HornerHashing> &set,
        const std::string &str
    ) {
        if (set.remove(str)) {
            std::cout << "OK" << std::endl;
        }
        else {
            std::cout << "FAIL" << std::endl;
        }
    };

    commands['?'] = []( 
        Set<std::string, HornerHashing> &set,
        const std::string &str
    ) {
        if (set.exist(str)) {
            std::cout << "OK" << std::endl;
        }
        else {
            std::cout << "FAIL" << std::endl;
        }
    };

    Set<std::string, HornerHashing> set;

    char cmd;
    std::string str;

    while (std::cin >> cmd >> str) {
        auto function = commands.at(cmd);
        function(set, str);
    }

    return 0;
}