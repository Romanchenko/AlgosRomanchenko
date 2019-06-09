#include <algorithm>
#include <cmath>
#include <utility>

template<class ValueType> class Set {
    private:
        struct Node {
            ValueType value;
            Node* left;
            Node* right;
            Node* parent;
            int height;
            size_t size;

            Node(
                const ValueType& value,
                Node* left = nullptr,
                Node* right = nullptr,
                Node* parent = nullptr,
                int height = 1,
                size_t size = 1
            )
                : value(value)
                , left(left)
                , right(right)
                , parent(parent)
                , height(height)
                , size(size)
            {}
        };

        int get_diff(Node* v) {
            if (v == nullptr) {
                return 0;
            }
            return get_h(v->left) - get_h(v->right);
        }

        int get_h(Node* node) const {
            if (node == nullptr) {
                return 0;
            }
            return node->height;
        }

        size_t get_sz(Node* node) const {
            if (node == nullptr) {
                return 0;
            }
            return node->size;
        }

        Node* root;

        std::pair<Node*, Node*> find_(const ValueType& value) const {
            if (root == nullptr) {
                return {nullptr, nullptr};
            }
            Node* cur = root;
            Node* prev = nullptr;
            while (cur != nullptr) {
                if (!(cur->value < value) && !(value < cur->value)) {
                    return {cur, prev};
                }
                prev = cur;
                if (cur->value < value) {
                    cur = cur->right;
                } else {
                    cur = cur->left;
                }
            }
            return {cur, prev};
        }

        void recalc(Node* v) {
            if (v == nullptr) {
                return;
            }
            v->height = std::max(get_h(v->left), get_h(v->right)) + 1;
            v->size = get_sz(v->left) + get_sz(v->right) + 1;
        }

        Node* small_left_rotation(Node* v) {
            Node* u = v->right;
            Node* p = v->parent;
            u->parent = p;
            if (p != nullptr) {
                if (p->left == v) {
                    p->left = u;
                } else {
                    p->right = u;
                }
            }
            v->right = u->left;
            if (u->left != nullptr) {
                u->left->parent = v;
            }
            u->left = v;
            v->parent = u;
            // recalc снизу вверх
            recalc(v);
            recalc(u);
            recalc(p);
            return u;
        }

        Node* small_right_rotation(Node* v) {
            Node* u = v->left;
            Node* p = v->parent;
            u->parent = p;
            if (p != nullptr) {
                if (p->left == v) {
                    p->left = u;
                } else {
                    p->right = u;
                }
            }
            v->left = u->right;
            if (u->right != nullptr) {
                u->right->parent = v;
            }
            u->right = v;
            v->parent = u;
            recalc(v);
            recalc(u);
            recalc(p);
            return u;
        }

        Node* big_left_rotation(Node* v) {
            Node* u = v->right;
            Node* w = u->left;
            Node* p = v->parent;
            w->parent = p;
            if (p != nullptr) {
                if (p->left == v) {
                    p->left = w;
                } else {
                    p->right = w;
                }
            }
            v->right = w->left;
            if (w->left != nullptr) {
                w->left->parent = v;
            }
            u->left = w->right;
            if (w->right != nullptr) {
                w->right->parent = u;
            }
            u->parent = w;
            v->parent = w;
            w->left = v;
            w->right = u;
            recalc(u);
            recalc(v);
            recalc(w);
            recalc(p);
            return w;
        }

        Node* big_right_rotation(Node* v) {
            Node* u = v->left;
            Node* w = u->right;
            Node* p = v->parent;
            w->parent = p;
            if (p != nullptr) {
                if (p->left == v) {
                    p->left = w;
                } else {
                    p->right = w;
                }
            }
            u->right = w->left;
            if (w->left != nullptr) {
                w->left->parent = u;
            }
            v->left = w->right;
            if (w->right != nullptr) {
                w->right->parent = v;
            }
            u->parent = w;
            v->parent = w;
            w->left = u;
            w->right = v;
            recalc(u);
            recalc(v);
            recalc(w);
            recalc(p);
            return w;
        }

        Node* rebalance(Node* v) {
            if (v == nullptr) {
                return nullptr;
            }
            int diff = get_diff(v);
            if (diff == -2) { // высота правого поддерева больше, чем левого, делаем один из левых поворотов
                if (get_diff(v->right) == -1 || get_diff(v->right) == 0) {
                    if (v == root) {
                        return root = small_left_rotation(v);
                    }
                    return small_left_rotation(v);
                } else {
                    if (v == root) {
                        return root = big_left_rotation(v);
                    }
                    return big_left_rotation(v);
                }
            } else {  // высота левого поддерева больше, чем правого, делаем один из правых поворотов
                if (get_diff(v->left) == 1 || get_diff(v->left) == 0) {
                    if (v == root) {
                        return root = small_right_rotation(v);
                    }
                    return small_right_rotation(v);
                } else {
                    if (v == root) {
                        return root = big_right_rotation(v);
                    }
                    return big_right_rotation(v);
                }
            }
        }

        void clear() {
            if (root == nullptr) {
                return;
            }
            Node* v = root;
            while (v != nullptr) {
                if (v->left == nullptr && v->right == nullptr) {
                    Node* next = v->parent;
                    if (next != nullptr && next->left == v) {
                        next->left = nullptr;
                    } else if (next != nullptr && next->right == v) {
                        next->right = nullptr;
                    }
                    delete v;
                    v = next;
                } else if (v->left == nullptr) {
                    v = v->right;
                } else {
                    v = v->left;
                }
            }
            root = nullptr;
        }

        Node* get_next(Node* v) const {
            if (v == nullptr) {
                return nullptr;
            }
            if (v->right != nullptr) {
                v = v->right;
                while (v->left != nullptr) {
                    v = v->left;
                }
                return v;
            }
            while (v->parent != nullptr && v->parent->right == v) {
                v = v->parent;
            }
            return v->parent;
        }

        Node* get_prev(Node* v) const {
            if (v == nullptr) {
                return nullptr;
            }

            if (v->left != nullptr) {
                v = v->left;
                while (v->right != nullptr) {
                    v = v->right;
                }
                return v;
            }
            while (v->parent != nullptr && v->parent->left == v) {
                v = v->parent;
            }
            return v->parent;
        }

        Node* get_most_left() const {
            if (root == nullptr) {
                return nullptr;
            }
            Node* v = root;
            while (v->left != nullptr) {
                v = v->left;
            }
            return v;
        }

        Node* get_most_right() const {
            if (root == nullptr) {
                return nullptr;
            }
            Node* v = root;
            while (v->right != nullptr) {
                v = v->right;
            }
            return v;
        }

    public:
        Set()
            : root(nullptr)
        {}

        template<class Iterator>
        Set(Iterator first, Iterator last)
            : root(nullptr) {
            while (first != last) {
                insert(*first);
                ++first;
            }
        }

        Set(const std::initializer_list<const ValueType>& init)
            : root(nullptr) {
            for (const auto& elem : init) {
                insert(elem);
            }
        }

        Set(const Set& other)
            : root(nullptr) {
            for (auto elem : other) {
                insert(elem);
            }
        }

        Set& operator = (const Set& other) {
            Set temp_set = Set(other.begin(), other.end());
            clear();
            for (auto elem : temp_set) {
                insert(elem);
            }
            return *this;
        }

        ~Set() {
            clear();
        }

        void insert(ValueType element) {
            Node* node = new Node(element);
            if (root == nullptr) {
                root = node;
                return;
            }
            // находим вершину, если такой элемент уже есть, и предка, к которому надо будет подвесить вершину
            std::pair<Node*, Node*> place = find_(element);

            if (place.first != nullptr) {
                delete node;
                return;
            }
            Node* v = place.second;
            node->parent = v;
            if (element < v->value) {
                v->left = node;
            } else {
                v->right = node;
            }
            recalc(v);
            while (v != nullptr) {
                int diff = get_diff(v);
                // Инвариант - размер какого-то поддерева v увеличился
                // До добавления вершины все балансы 0, 1 или -1
                // Если баланс стал равен нулю, то высота меньшего поддерева стала равна высоте большего,
                // высота поддерева с корнем в v не изменилась. Тогда можно прекратить балансировку.
                // Если баланс стал равен 1 или -1, то он был 0. Значит,
                // одно поддерево стало выше другого, продолжаем балансировку.
                // Если баланс стал равен 2 или -2, то надо сделать один из поворотов. 
                // Тип поворота определяется внутри функции rebalance, 
                // которая возвращает новый корень поддерева.
                if (diff == 0) {
                    break;
                }
                if (diff == 1 || diff == -1) {
                    recalc(v);
                    v = v->parent;
                } else {
                    v = rebalance(v);
                }
            }
            // дойдем до корня, чтобы пересчитать размеры
            while (v != nullptr) {
                recalc(v);
                v = v->parent;
            }
        }

        void erase_(Node* v) {
            if (v == nullptr) {
                return;
            }
            // рекурсивное удаление : если лист, то удалим его,
            // иначе найдем блидайший больший или меньший элемент
            // и запустим удаление от него, поменяв его с удаляемым
            if (v->left == nullptr && v->right == nullptr) {
                Node* p = v->parent;
                // если в дереве была одна вершина
                if (p == nullptr) {
                    root = nullptr;
                    delete v;
                    return;
                }
                if (p->left == v) {
                    p->left = nullptr;
                } else {
                    p->right = nullptr;
                }
                recalc(p);
                delete v;
                v = p;
                while (v != nullptr) {
                     // Инвариант - размер какого-то поддерева v уменьшился.
                     // До удаления вершины баланс v был корректным.
                     // Если он стал равен +1 или -1, то был равен нулю. Тогда размер поддерева с корнем в v не изменился, выходим.
                     // Если он стал равен 0, то был равен +1 или -1. Значит, размер поддерева v учменьшился, надо продолжать балансировку.
                     // Если баланс стал 2 или -2, то надо делать один из поворотов. Какой именно нужен - определяется внутри rebalance
                     // Сам rebalance вернет вершину, которая станет корнем поддерева.
                     
                    if (get_diff(v) == 1 || get_diff(v) == -1) {
                        break;
                    }
                    if (get_diff(v) == 0) {
                        recalc(v);
                        v = v->parent;
                    } else {
                        v = rebalance(v);
                    }
                }
                // пройдем до корня, чтобы пересчитать размеры
                while (v != nullptr) {
                    recalc(v);
                    v = v->parent;
                }
                return;
            }
            if (get_diff(v) >= 0) {
                Node* u = get_prev(v);
                std::swap(u->value, v->value);
                erase_(u);
            } else {
                Node* u = get_next(v);
                std::swap(u->value, v->value);
                erase_(u);
            }
        }

        void erase(const ValueType& elem) {
            std::pair<Node*, Node*> found = find_(elem);
            Node* v = found.first;
            if (v == nullptr) {
                return;
            }
            erase_(v);
        }

        size_t size() const {
            return get_sz(root);
        }

        bool empty() const {
            return get_sz(root) == 0;
        }

        class iterator;

        iterator begin() const {
            return iterator(this, get_most_left());
        }

        iterator end() const {
            return iterator(this, nullptr);
        }

        iterator find(const ValueType& elem) const {
            std::pair<Node*, Node*> found = find_(elem);
            return iterator(this, found.first);
        }

        iterator lower_bound(const ValueType& elem) const {
            std::pair<Node*, Node*> found = find_(elem);
            if (found.first != nullptr) {
                return iterator(this, found.first);
            }
            Node* p = found.second;
            if (p == nullptr) {
                return iterator(this, nullptr);
            }
            if (elem < p->value) {
                return iterator(this, p);
            }
            p = get_next(p);
            if (p == nullptr) {
                return iterator(this, nullptr);
            }
            return iterator(this, p);
        }

        class iterator {
            public:
                iterator(const Set* set = nullptr, Node* ptr = nullptr)
                    : set_(set)
                    , ptr_(ptr)
                {}

                iterator(const iterator& other)
                    : set_(other.set_)
                    , ptr_(other.ptr_)
                {}

                iterator& operator = (const iterator& other) {
                    ptr_ = other.ptr_;
                    set_ = other.set_;
                    return *this;
                }

                iterator& operator ++() {
                    ptr_ = set_->get_next(ptr_);
                    return *this;
                }

                iterator operator ++(int _) {
                    iterator tmp(*this);
                    ptr_ = set_->get_next(ptr_);
                    return tmp;
                }

                iterator& operator --() {
                    if (ptr_ == nullptr) {
                        ptr_ = set_->get_most_right();
                    } else {
                        ptr_ = set_->get_prev(ptr_);
                    }
                    return *this;
                }

                iterator operator --(int _) {
                    iterator tmp(*this);
                    if (ptr_ == nullptr) {
                        ptr_ = set_->get_most_right();
                    } else {
                        ptr_ = set_->get_prev(ptr_);
                    }
                    return tmp;
                }

                bool operator == (const iterator& other) const {
                    return ptr_ == other.ptr_;
                }

                bool operator != (const iterator& other) const {
                    return ptr_ != other.ptr_;
                }

                const ValueType* operator -> () const {
                    return &(ptr_->value);
                }

                const ValueType& operator * () const {
                    return ptr_->value;
                }

            private:
                const Set* set_;
                Node* ptr_;
        };
};
