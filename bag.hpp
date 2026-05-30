template <typename F>
struct bag {
    private:
    struct node {
        node(){};
        node(F const& v, node* n) : val(v) {next=n;}
        F val;
        node* next;
    };
    node* head;
    public:
    struct bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = F;
        using pointer = F*;
        using reference = F&;

        bag_iterator(node*);
        reference operator*() const;
        pointer operator->() const;
        bag_iterator& operator++();
        bag_iterator operator++(int);
        bool operator==(bag_iterator const&) const;
        bool operator!=(bag_iterator const&) const;
    private:
        node* m_ptr;
    };

    struct const_bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const F;
        using pointer = F const*;
        using reference = F const&;

        const_bag_iterator(node const*);
        reference operator*() const;
        pointer operator->() const;
        const_bag_iterator& operator++();
        const_bag_iterator operator++(int);
        bool operator==(const_bag_iterator const&) const;
        bool operator!=(const_bag_iterator const&) const;
    private:
        node const* m_ptr;
    };
    bag();
    bag(bag<F> const& other);
    ~bag();
    void destroy_children();
    void add_child(F const& child);
    std::ostream& print(std::ostream&) const;
    bag<F>& operator=(bag<F> const& other);
    bool operator==(bag<F> const& other) const;
    void delete_child(F t);
    void remove_compressed();
    void sort(F& t);
    bag_iterator begin();
    bag_iterator end();
    const_bag_iterator begin() const;
    const_bag_iterator end() const;
    bool isLeaf() const;
};

template <typename F>
bag<F>::bag() : head(nullptr) {}

template <typename F>
bag<F>::bag(bag<F> const& other) {
    head = new node{other.head->val, nullptr};
    node* pc = head;
    node* po = other.head;
    if (po) po->next;
    while (po) {
        pc->next = new node{po->val, nullptr};
        pc=pc->next;
        po=po->next;
    }
}

template <typename F>
bag<F>::~bag() {
    while (head) {
        node* tmp = head;
        head=head->next;
        delete tmp;
    }
}

template <typename F>
void bag<F>::destroy_children() {
    while (head) {
        node* tmp = head;
        head=head->next;
        delete tmp;
    }
}

template <typename F>
bool bag<F>::isLeaf() const {
    return (head==nullptr);
}

template <typename F>
void bag<F>::add_child(F const& child) {
    node* pc = head;
    while (pc) { // check equal labels
        if (*pc->val.get_label() == *child.get_label()) {
            head=nullptr;
            return;
        }
        pc = pc->next;
    }
    if (!head) {
        head = new node{child, nullptr};
        }
    else if (*child.get_label() < *head->val.get_label()) {
        node *new_node = new node{child, head};
        head = new_node;
    } else {
        pc = head;
        while (pc->next && *child.get_label() > *pc->next->val.get_label())
            pc = pc->next;
        node *new_node = new node{child, pc->next};
        pc->next = new_node;
    }
}

template <typename F>
std::ostream& bag<F>::print(std::ostream& os) const {
    node* pc = head;
     while (pc) {
        os << pc->val;
        if (pc->next) os << ", ";
        pc = pc->next;
    }
    return os;
}

template <typename F>
bag<F> &bag<F>::operator=(bag<F> const &other) {
    if (this != &other) {
        while (head) {
            node* tmp = head;
            head = head->next;
            delete tmp;
            }
        if (other.head) {
            head = new node{other.head->val, nullptr};
            node* pc = head;
            node* po = other.head->next;
            while (po) {
                pc->next = new node{po->val, nullptr};
                pc = pc->next;
                po = po->next;
            }
        }
    }
    return *this;
}

template <typename F>
bool bag<F>::operator==(bag<F> const& other) const {
    node* pc = head;
    node* po = other.head;
    bool uguali=true;
    while (pc && po && uguali) {
        if (pc->val != po->val) uguali=false;
        pc=pc->next;
        po=po->next;
    }
    return (uguali && !pc && !po); // return if equal and both at nullptr
}

template <typename F>
void bag<F>::remove_compressed() {
    assert(!head->next); // stop the program if head->next exists
    delete head;
    head=nullptr;
}

template <typename F>
void bag<F>::delete_child(F t) {
    if (*head->val.get_label()==*t.get_label()) {
        node* tmp = head;
        head = head->next;
        delete tmp;
        add_child(t);
    } else {
    node* pc = head;
    while (pc->next && *pc->next->val.get_label()!=*t.get_label()) pc=pc->next;
        node* tmp = pc->next;
        pc->next = tmp->next;
        delete tmp;

    }

}
template <typename F>
void bag<F>::sort(F& t) {
    if (head && head->next) {
            node* tmp;
            bool fix=false;
        if (*head->val.get_label() == *t.get_label()) {
            if (*t.get_label()>*head->next->val.get_label()) {
            tmp = head;
            head = head->next;
            fix=true;
            }
        } else {
            node* pc = head;
            while (*pc->next->val.get_label() != *t.get_label()) pc=pc->next;
            if (pc->next->next && (*t.get_label() > *pc->next->next->val.get_label() || *t.get_label() < *pc->val.get_label())) {
                tmp=pc->next;
                pc->next=pc->next->next;
                fix=true;
            }
        }
        if (fix==true) {
            if (*head->val.get_label() > *t.get_label()) {
                tmp->next = head;
                head=tmp->next;
            }
            else {
                node* pc = head;
                while (pc->next && *pc->next->val.get_label() < *t.get_label()) pc = pc->next;
                tmp->next = pc->next;
                pc->next = tmp;
            }
        }
    }
}

template <typename F>
bag<F>::bag_iterator::bag_iterator(node* ptr) : m_ptr(ptr) {}

template <typename F>
F& bag<F>::bag_iterator::operator*() const {
    return m_ptr->val;
}

template <typename F>
F* bag<F>::bag_iterator::operator->() const {
    return &m_ptr->val;
}

template <typename F>
typename bag<F>::bag_iterator& bag<F>::bag_iterator::operator++() {
    m_ptr = m_ptr->next;
    return *this;
}

template <typename F>
typename bag<F>::bag_iterator bag<F>::bag_iterator::operator++(int) {
    bag_iterator temp = *this;
    m_ptr = m_ptr->next;
    return temp;
}

template <typename F>
bool bag<F>::bag_iterator::operator==(bag_iterator const& other) const {
    return m_ptr == other.m_ptr;
}

template <typename F>
bool bag<F>::bag_iterator::operator!=(bag_iterator const& other) const {
    return m_ptr != other.m_ptr;
}

template <typename F>
typename bag<F>::bag_iterator bag<F>::begin() {
    return bag_iterator{head};
}

template <typename F>
typename bag<F>::bag_iterator bag<F>::end() {
    return bag_iterator{nullptr};
}

//const

template <typename F>
bag<F>::const_bag_iterator::const_bag_iterator(node const* ptr) : m_ptr(ptr) {}

template <typename F>
F const& bag<F>::const_bag_iterator::operator*() const {
    return m_ptr->val;
}

template <typename F>
F const* bag<F>::const_bag_iterator::operator->() const {
    return &m_ptr->val;
}

template <typename F>
typename bag<F>::const_bag_iterator& bag<F>::const_bag_iterator::operator++() {
    m_ptr = m_ptr->next;
    return *this;
}

template <typename F>
typename bag<F>::const_bag_iterator bag<F>::const_bag_iterator::operator++(int) {
    const_bag_iterator temp = *this;
    m_ptr = m_ptr->next;
    return temp;
}

template <typename F>
bool bag<F>::const_bag_iterator::operator==(const_bag_iterator const& other) const {
    return m_ptr == other.m_ptr;
}

template <typename F>
bool bag<F>::const_bag_iterator::operator!=(const_bag_iterator const& other) const {
    return m_ptr != other.m_ptr;
}

template <typename F>
typename bag<F>::const_bag_iterator bag<F>::begin() const {
    return const_bag_iterator{head};
}

template <typename F>
typename bag<F>::const_bag_iterator bag<F>::end() const {
    return const_bag_iterator{nullptr};
}
