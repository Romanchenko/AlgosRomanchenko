#include <cstdio>
#include <iterator>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

size_t kMaxSize = (1 << 10);

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    typedef typename std::pair<const KeyType, ValueType> HashPair;
    typedef typename std::list<HashPair>::iterator ListIterator;
    typedef typename std::list<HashPair>::const_iterator ConstListIterator;

    private:
        size_t size_;
        Hash hasher_;
        std::vector<ListIterator> borders_;
        std::list<HashPair> data_;

        size_t local_hash(const KeyType& value) const {
            return hasher_(value) % kMaxSize;
        }

        void resize() {
            kMaxSize *= 2;
            size_ = 0;
            std::list<HashPair> data_copy = data_;
            data_.clear();
            borders_.clear();
            borders_.resize(kMaxSize, ListIterator(nullptr));
            for (auto element : data_copy) {
                insert(element);
            }
        }

    public:
        HashMap(Hash hash_func = Hash())
            : size_(0)
            , hasher_(hash_func)
            , borders_(kMaxSize, ListIterator(nullptr))
        {}

        HashMap(const HashMap& other, Hash hash_func = Hash())
            : size_(0)
            , hasher_(hash_func)
            , borders_(kMaxSize, ListIterator(nullptr)) {
            for (auto element : other) {
                insert(element);
            }
        }

        template<class Inputiterator>
        HashMap(Inputiterator In, Inputiterator Out, Hash hash_func = Hash())
            : size_(0)
            , hasher_(hash_func)
            , borders_(kMaxSize, ListIterator(nullptr)) {
            while (In != Out) {
                insert(*In);
                ++In;
            }
        }

        HashMap(const std::initializer_list<const HashPair>& init_list,
                Hash hash_func = Hash())
            : size_(0)
            , hasher_(hash_func)
            , borders_(kMaxSize, ListIterator(nullptr)) {
            for (auto element : init_list) {
                insert(element);
            }
        }

        HashMap& operator = (const HashMap& other) {
            HashMap tmp_hashmap(other.begin(), other.end());
            clear();
            for (auto element : tmp_hashmap) {
                insert(element);
            }
            return *this;
        }

        size_t size() const {
            return size_;
        }

        bool empty() const {
            return size_ == 0;
        }

        Hash hash_function() const {
            return hasher_;
        }

        class iterator;
        class const_iterator;

        iterator find(const KeyType& key) {
            size_t key_hash = local_hash(key);
            if (borders_[key_hash] == ListIterator(nullptr)) {
                return end();
            }
            ListIterator it = borders_[key_hash];
            while (it != data_.end() && local_hash((*it).first) == key_hash) {
                if ((*it).first == key) {
                    return iterator(it);
                }
                ++it;
            }
            return end();
        }

        const_iterator find(const KeyType& key) const {
            size_t key_hash = local_hash(key);
            if (borders_[key_hash] == ListIterator(nullptr)) {
                return end();
            }
            ListIterator it = borders_[key_hash];
            while (it != data_.end() && local_hash((*it).first) == key_hash) {
                if ((*it).first == key) {
                    return const_iterator(it);
                }
                ++it;
            }
            return end();
        }


        void insert(const HashPair& element) {
            if (find(element.first) != end()) {
                return;
            }
            size_t key_hash = local_hash(element.first);
            if (data_.empty()) {
                data_.push_back(element);
                borders_[key_hash] = data_.end();
                ++size_;
                --borders_[key_hash];
                return;
            }
            if (borders_[key_hash] == ListIterator(nullptr)) {
                borders_[key_hash] = data_.end();
            }
            ListIterator it = borders_[key_hash];
            data_.insert(it, element);
            --borders_[key_hash];
            ++size_;
            if (size_ * 2 >= kMaxSize) {
                resize();
            }
        }

        void erase(const KeyType& key) {
            size_t key_hash = local_hash(key);
            ListIterator it = borders_[key_hash];
            if (it == ListIterator(nullptr)) {
                return;
            }
            if ((*it).first == key) {
                ListIterator it2 = it;
                ++it2;
                data_.erase(it);
                --size_;
                if (it2 == data_.end() || local_hash((*it2).first) != key_hash) {
                    borders_[key_hash] = ListIterator(nullptr);
                } else {
                    borders_[key_hash] = it2;
                }
            } else {
                while (it != data_.end() && local_hash((*it).first) == key_hash) {
                    if ((*it).first == key) {
                        data_.erase(it);
                        --size_;
                        break;
                    }
                    ++it;
                }
            }
        }

        void clear() {
            size_ = 0;
            for (auto & it : data_) {
                borders_[local_hash(it.first)] = ListIterator(nullptr);
            }
            data_.clear();
        }

        ValueType& operator[] (const KeyType& key) {
            iterator it = find(key);
            if (it == end()) {
                insert(std::make_pair(key, ValueType()));
                it = find(key);
            }
            return (*it).second;
        }

        const ValueType& at(const KeyType& key) const {
            const_iterator it = find(key);
            if (it == end()) {
                throw std::out_of_range("");
            }
            return (*it).second;
        }

        iterator begin() {
            return iterator(data_.begin());
        }

        const_iterator begin() const {
            return const_iterator(data_.begin());
        }

        iterator end() {
            return iterator(data_.end());
        }

        const_iterator end() const {
            return const_iterator(data_.end());
        }

        class iterator {
            private:
                ListIterator ptr;

            public:
                iterator(ListIterator ptr = ListIterator(nullptr))
                    : ptr(ptr)
                {}

                iterator(const iterator& other)
                    : ptr(other.ptr)
                {}

                iterator& operator = (const iterator& other) {
                    ptr = other.ptr;
                    return *this;
                }


                iterator& operator ++() {
                    ++ptr;
                    return *this;
                }

                iterator operator ++(int _) {
                    iterator tmp = *this;
                    ++ptr;
                    return tmp;
                }

                iterator& operator --() {
                    --ptr;
                    return *this;
                }

                iterator operator --(int _) {
                    iterator tmp = *this;
                    --ptr;
                    return tmp;
                }

                HashPair& operator * () {
                    return *ptr;
                }

                HashPair* operator -> () {
                    return &(*ptr);
                }

                bool operator == (const iterator& other) const {
                    return ptr == iterator(other).ptr;
                }

                bool operator != (const iterator& other) const {
                    return !(*this == other);
                }

                bool operator == (const const_iterator& other) const {
                    return ptr == iterator(other).ptr;
                }

                bool operator != (const const_iterator& other) const {
                    return ptr != const_iterator(other).ptr;
                }
        };

        class const_iterator {
            private:
                ConstListIterator ptr;

            public:
                const_iterator(ConstListIterator ptr = ConstListIterator(nullptr))
                        : ptr(ptr)
                    {}

                const_iterator& operator = (const const_iterator& other) {
                    ptr = other.ptr;
                    return *this;
                }

                const_iterator& operator ++() {
                    ++ptr;
                    return *this;
                }

                const_iterator operator ++(int _) {
                    const_iterator tmp = *this;
                    ++ptr;
                    return tmp;
                }

                const_iterator& operator --() {
                    --ptr;
                    return *this;
                }

                const_iterator operator --(int _) {
                    iterator tmp = *this;
                    --ptr;
                    return tmp;
                }

                const HashPair& operator * () const {
                    return *ptr;
                }

                const HashPair* operator -> () const {
                    return &(*ptr);
                }

                bool operator == (const const_iterator& other) const {
                    return ptr == other.ptr;
                }

                bool operator != (const const_iterator& other) const {
                    return !(*this == other);
                }

                bool operator == (const iterator& other) const {
                    return ptr == other.ptr;
                }

                bool operator != (const iterator& other) const {
                    return ptr != other.ptr;
                }
        };
};
