#include "trie.hpp"

template <typename T>
std::ostream& operator<<(std::ostream& os, trie<T> const& t) {
    if (t.get_label()!=nullptr) os << *t.get_label() << " "; // prints label
    if (t.get_children().isLeaf()) os << t.get_weight() << " children = {}"; // prints weight if leaf
    else {
        os << "children = { ";
        t.get_children().print(os);
        os << "}";
    }
    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, trie<T>& t) {
    t.get_children().destroy_children();
    is >> std::ws;
    char c;
    c = is.peek();
    double weight = 0.0;
    bool leaf = false;
    if ((c >= '0' && c <= '9') || c=='-') {
        is >> weight;
        is >> std::ws;
        leaf = true;
    }
    char childrenword[8];
    is.read(childrenword, 8);
    std::string contentstring;
    for (int i=0;i<8;i++) contentstring += childrenword[i];
    if (contentstring!="children") throw parser_exception("expected key word children or label not expected\n");
    is >> std::ws;
    is.get(c);
    if (c!='=') throw parser_exception("expected '='\n");
    is >> std::ws;
    is.get(c);
    if (c!='{') throw parser_exception("expected '{'\n");
    is >> std::ws;
    c = is.peek();
    if (c!='}') { // has children
        if (leaf) throw parser_exception("weight not expected because trie is a parent\n");
        else {
            do {
            is >> std::ws;
            T label;
            is >> label;
            trie<T> child;
            child.set_parent(&t);
            child.set_label(&label);
            t.add_child(child);
            std::vector<T> s{label};
            is >> t[s];
            is >> std::ws;
            is.get(c); // take , or }
            } while (c==',');
            if (c!='}') throw parser_exception("expected } or ,\n");
        }
    }
    else { // does not have children
        is.ignore(1);
        if (!leaf) throw parser_exception("weight not found\n");
        else
            t.set_weight(weight);
    }
    return is;
}

//CONSTRUCTORS
template <typename T>
trie<T>::trie() : m_p(nullptr), m_l(nullptr), m_w(0.0) {}

template <typename T>
trie<T>::trie(double weight) : m_p(nullptr), m_l(nullptr), m_w(weight) {}

template <typename T>
trie<T>::~trie() {
    delete m_l;
}

template <typename T>
trie<T>::trie(trie<T> const& other) : m_p(other.m_p), m_w(other.m_w) { // copy constr
    m_c = other.m_c;
    if (other.m_l) {
    m_l = new T;
    *m_l = *other.m_l;
    } else m_l=nullptr;
}

template <typename T> // move constr
trie<T>::trie(trie<T>&& other) : m_p(other.m_p), m_w(other.m_w) {
    m_c = other.m_c;
    other.m_c.destroy_children();
    m_l=other.m_l;
    other.m_p = nullptr;
    other.m_l = nullptr;
    other.m_w = 0.0;
}


//ASSIGNMENTS
template <typename T>
trie<T>& trie<T>::operator=(trie<T> const& other) { // copy assignment
    if (this != &other) {
        m_c = other.m_c;
        m_w = other.m_w;
    }
    return *this;
}

template <typename T> // move assignment
trie<T>& trie<T>::operator=(trie<T>&& other) {
    if (this != &other) {
        m_c = other.m_c;
        other.m_c.destroy_children();
        m_w = other.m_w;
        other.m_p = nullptr;
        other.m_w = 0.0;
    }
    return *this;
}


//GETTER SETTER

template <typename T>
void trie<T>::set_weight(double w) {
    m_w = w;
}

template <typename T>
double trie<T>::get_weight() const {
    return m_w;
}

template <typename T>
void trie<T>::set_label(T* l) {
    if (m_l==nullptr) m_l = new T;
    *m_l = *l;
}

template <typename T>
T const* trie<T>::get_label() const {
    return m_l;
}

template <typename T>
T* trie<T>::get_label() {
    return m_l;
}

template <typename T>
void  trie<T>::set_parent(trie<T>* p) {
    m_p = p;
}

template <typename T>
trie<T> const*  trie<T>::get_parent() const {
    return m_p;
}

template <typename T>
trie<T>* trie<T>::get_parent() {
    return m_p;
}

template <typename T>
bag<trie<T>> const& trie<T>::get_children() const {
    return m_c;
}

template <typename T>
bag<trie<T>>& trie<T>::get_children() {
    return m_c;
}

template <typename T>
void trie<T>::add_child(trie<T> const& c) {
    if (m_w!=0.0) m_w=0.0;
    m_c.add_child(c);
    if (m_c.isLeaf()) throw parser_exception("2 equal labels in the same sub-trie");
}

template <typename T>
bool trie<T>::operator==(trie<T> const& other) const {
    return (m_c==other.m_c && (!m_c.isLeaf() || m_w==other.m_w));
}

template <typename T>
bool trie<T>::operator!=(trie<T> const& other) const {
    return !(*this==other);
}

template <typename T>
trie<T>& trie<T>::operator[](std::vector<T> const& s) {
    typename bag<trie<T>>::bag_iterator it = m_c.begin();
    while (it!=m_c.end() && (it->get_label() && *it->get_label()!=s.at(0))) ++it;
    if (it!=m_c.end() && s.size()>1) return (*it)[std::vector<T>(s.begin() + 1, s.end())]; // recursion on the children element with correct label and on the next element of the array
    else if (it!=m_c.end()) return *it; //if i checked the last position of the array and found the correct label, return pointer to the correct tree
    else return *this; // if i did not find the label, i return the tree
}

template <typename T>
trie<T> const& trie<T>::operator[](std::vector<T> const& s) const {
    typename bag<trie<T>>::const_bag_iterator it = m_c.begin();
    while (it!=m_c.end() && (it->get_label() && *it->get_label()!=s.at(0))) ++it;
    if (it!=m_c.end() && s.size()>1) return (*it)[std::vector<T>(s.begin() + 1, s.end())];
    else if (it!=m_c.end()) return *it;
    else return *this;
}

//3.8 max
template <typename T>
trie<T>& trie<T>::max() {
    leaf_iterator leaf_record = this->begin();
    for (leaf_iterator leaf_it = this->begin(); leaf_it != this->end(); ++leaf_it) {
            if (leaf_it.get_leaf().get_weight()>leaf_record.get_leaf().get_weight()) leaf_record = leaf_it;
            }
    return leaf_record.get_leaf();
}

template <typename T>
trie<T> const& trie<T>::max() const {
    const_leaf_iterator leaf_record = this->begin();
    for (const_leaf_iterator leaf_it = this->begin(); leaf_it != this->end(); ++leaf_it)
            if (leaf_it.get_leaf().get_weight()>leaf_record.get_leaf().get_weight()) leaf_record = leaf_it;
    return leaf_record.get_leaf();
}

//3.9.1 UNION

template <typename T>
trie<T> trie<T>::operator+(trie<T> const& other) const {
    trie<T> t; // trie to return
    if (this->get_children().isLeaf() && other.get_children().isLeaf()) // if both leaf
        t.set_weight(this->get_weight() + other.get_weight()); // return sum of the weight

    else if (this->get_children().isLeaf() && !other.get_children().isLeaf()) { // if one has got leaf
        for (typename bag<trie<T>>::const_bag_iterator it = other.m_c.begin();it != other.m_c.end();++it) t.add_child(*it); // for every children, add the children to t

        for (typename bag<trie<T>>::bag_iterator it = t.m_c.begin();it != t.m_c.end();++it) { // for every children of t, fix the parent
                it->set_parent(&t);
                /*if (t.get_parent()) {

                            std::vector<T> g = {*t.get_label()};

                            it->set_parent(&(*t.get_parent())[g]);
                        } else it->set_parent(&t);*/
        }

        for (leaf_iterator leaf_it = t.begin(); leaf_it != t.end(); ++leaf_it) // for every leaf of t, fix the weight
                leaf_it.get_leaf().set_weight(leaf_it.get_leaf().get_weight() + this->get_weight());

    } else if (!this->get_children().isLeaf() && other.get_children().isLeaf()) {
        for (typename bag<trie<T>>::const_bag_iterator it = m_c.begin();it != m_c.end();++it) t.add_child(*it);

        for (typename bag<trie<T>>::bag_iterator it = t.m_c.begin();it != t.m_c.end();++it) {
                it->set_parent(&t);
                /*if (t.get_parent()) {

                            std::vector<T> g = {*t.get_label()};

                            it->set_parent(&(*t.get_parent())[g]);

                        } else it->set_parent(&t);*/
        }

        for (leaf_iterator leaf_it = t.begin(); leaf_it != t.end(); ++leaf_it)
                leaf_it.get_leaf().set_weight(leaf_it.get_leaf().get_weight() + other.get_weight());

    } else {// if both have children
        for (typename bag<trie<T>>::const_bag_iterator it = m_c.begin();it != m_c.end();++it) { //controllo se this ha figli con label uguali alle label dei figli di other
                bool found = false;
            for (typename bag<trie<T>>::const_bag_iterator it2 = other.m_c.begin();it2 != other.m_c.end() && !found;++it2) {
                if (*it->get_label() == *it2->get_label()) {//se label uguali
                        const_node_iterator node_it = &(*it), node_it2 = &(*it2);
                        const trie<T>*root = &(*it);
                        const trie<T>*root2 = &(*it2);
                        while (root->get_parent()) root = root->get_parent();
                        while (root2->get_parent()) root2 = root2->get_parent();
                        while (node_it != const_node_iterator{root->root()} && node_it2 != const_node_iterator{root2->root()} && *node_it == *node_it2) {
                            ++node_it;
                            ++node_it2;
                        }
                    if (node_it == const_node_iterator{root->root()} && node_it2 == const_node_iterator{root2->root()}) {
                        trie<T> tmp;
                        T label = *it->get_label();
                        tmp.set_label(&label);
                        if (t.get_parent()) {
                            std::vector<T> g = {*t.get_label()};
                            tmp.set_parent(&(*t.get_parent())[g]);
                        } else tmp.set_parent(&t);
                        tmp = (*it) + (*it2); //ricorsione
                        t.add_child(tmp);
                        std::vector<T> s = {*tmp.get_label()};
                        if (t.get_parent()) {
                            std::vector<T> g = {*t.get_label()};
                            t[s].set_parent(&(*t.get_parent())[g]);
                        } else t[s].set_parent(&t);
                        found = true;
                    }
                }
            }
            if (!found) { //se nessuna label uguale, aggiungo figlio e sistemo parent
                    t.add_child(*it);
                    std::vector<T> s = {*it->get_label()};
                    if (t.get_parent()) {
                            std::vector<T> g = {*t.get_label()};
                            t[s].set_parent(&(*t.get_parent())[g]);
                        } else t[s].set_parent(&t);
            }
        }

        for (typename bag<trie<T>>::const_bag_iterator it = other.m_c.begin();it != other.m_c.end();++it) { //controllo secondo, uguale a sopra
                bool found = false;
            for (typename bag<trie<T>>::const_bag_iterator it2 = m_c.begin();it2 != m_c.end() && !found;++it2) {
                if (*it->get_label() == *it2->get_label()) found = true;
            }
            if (!found) {
                    t.add_child(*it);
                    std::vector<T> s = {*it->get_label()};
                    if (t.get_parent()) {
                            std::vector<T> g = {*t.get_label()};
                            t[s].set_parent(&(*t.get_parent())[g]);
                        } else t[s].set_parent(&t);
            }
        }
    }
    return t;
}

template <typename T>
trie<T>& trie<T>::operator+=(trie<T> const& other) {
    *this = *this + other;
    return *this;
}

//3.9.2 COMPATTAZIONE

template <typename T>
void trie<T>::path_compress() {
        int i=0;
    for (typename bag<trie<T>>::bag_iterator it = m_c.begin();it!=m_c.end();++it) { //per ogni figlio faccio ricorsione
        it->path_compress();
        ++i;
    }
    if (i==1 && m_l) {//se ha un solo children e ha una label(quindi non è radice)
        typename bag<trie<T>>::bag_iterator it_compressed = m_c.begin();
        trie<T>* compressed = &(*it_compressed); //trie da comprimere
        *m_l += *compressed->m_l;
        m_w = compressed->m_w;

        for (typename bag<trie<T>>::bag_iterator it = compressed->m_c.begin();it!=compressed->m_c.end();++it) { //per ogni figlio del trie da comprimere
            it->set_parent(this); //cambio padre
            add_child(*it); //aggiungo il figlio di compressed a this
        }
        m_c.delete_child(*compressed); //cancello compressed come figlio
    }

    if (this->get_parent()) {
        /*typename bag<trie<T>>::bag_iterator it = this->get_parent->m_c.begin();
        while (it->get_label() && it->get_label() != this->get_label()) ++it;*/
            //this->get_parent()->m_c.sort(*this);

    }
    /*
     for (typename bag<trie<T>>::bag_iterator it = m_c.begin();it!=m_c.end();++it) {
        //m_c.delete_child(*it);

        //add_child(tmp);

     }*///risistemo label

    /*int i=0;
    for (typename bag<trie<T>>::bag_iterator it = m_c.begin();it!=m_c.end();++it) {

        it->path_compress();
        ++i;
    }
    if (i==1 && m_l) {//se ha un solo children e ha una label(quindi non è radice)

        typename bag<trie<T>>::bag_iterator it_compressed = m_c.begin();

        trie<T> compressed = *it_compressed;
        *m_l += *compressed.m_l;
        m_w=compressed.m_w;
        m_c.remove_compressed();

        for (typename bag<trie<T>>::bag_iterator it = compressed.m_c.begin();it!=compressed.m_c.end();++it) {
            it->set_parent(this);
            add_child(*it);
        }

    }

    for (typename bag<trie<T>>::bag_iterator it = m_c.begin();it!=m_c.end();++it) {

        //trie<T>tmp = *it;
        for (typename bag<trie<T>>::bag_iterator it = m_c.begin();it!=m_c.end();++it) {
        m_c.delete_child(*it);}

        //add_child(tmp);
    }*/
}

//3.7 ITERATORI
// node iterator

template <typename T>
trie<T>::node_iterator::node_iterator(trie<T>* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::node_iterator trie<T>::root() {
    return node_iterator{this};
}

template <typename T>
T& trie<T>::node_iterator::operator*() const {
    return *m_ptr->m_l;
}

template <typename T>
T* trie<T>::node_iterator::operator->() const {
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::node_iterator& trie<T>::node_iterator::operator++() {
    m_ptr=m_ptr->m_p;
    return *this;
}

template <typename T>
typename trie<T>::node_iterator trie<T>::node_iterator::operator++(int) {
    auto temp = m_ptr;
    m_ptr=m_ptr->m_p;
    return temp;
}

template <typename T>
bool trie<T>::node_iterator::operator==(node_iterator const& other) const {
    return m_ptr==other.m_ptr;
}

template <typename T>
bool trie<T>::node_iterator::operator!=(node_iterator const& other) const {
    return m_ptr!=other.m_ptr;
}


// STATIC node iterator

template <typename T>
trie<T>::const_node_iterator::const_node_iterator(trie<T> const* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::root() const {
    return const_node_iterator{this};
}

template <typename T>
T const& trie<T>::const_node_iterator::operator*() const {
    return *m_ptr->m_l;
}

template <typename T>
T const* trie<T>::const_node_iterator::operator->() const {
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::const_node_iterator& trie<T>::const_node_iterator::operator++() {
    m_ptr=m_ptr->m_p;
    return *this;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::const_node_iterator::operator++(int) {
    auto temp = m_ptr;
    m_ptr=m_ptr->m_p;
    return temp;
}

template <typename T>
bool trie<T>::const_node_iterator::operator==(const_node_iterator const& other) const {
    return m_ptr==other.m_ptr;
}

template <typename T>
bool trie<T>::const_node_iterator::operator!=(const_node_iterator const& other) const {
    return m_ptr!=other.m_ptr;
}


//leaf iterator
template <typename T>
trie<T>::leaf_iterator::leaf_iterator(trie<T>* ptr) : m_ptr(ptr){}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::begin() {
    trie<T>* node = this;
    while (!node->m_c.isLeaf()) {
        node = &(*node->m_c.begin());
    }
    return leaf_iterator{node};
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::end() {
    if (!this->get_parent()) return leaf_iterator{nullptr};
    else {
        typename bag<trie<T>>::bag_iterator it = this->get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_label()) ++it;
        typename bag<trie<T>>::bag_iterator tmp = it;
        ++it;
        if (it==this->get_parent()->get_children().end()) return tmp->get_parent()->end();
        else return leaf_iterator{it->begin()};
    }
}

template <typename T>
trie<T>& trie<T>::leaf_iterator::get_leaf() const {
    return *m_ptr;
}

template <typename T>
T& trie<T>::leaf_iterator::operator*() const {
    return *m_ptr->m_l;
}

template <typename T>
T* trie<T>::leaf_iterator::operator->() const {
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::leaf_iterator& trie<T>::leaf_iterator::operator++() {
    if (this->get_leaf().get_parent()) { //se ha il padre
        typename bag<trie<T>>::bag_iterator it = this->get_leaf().get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_leaf().get_label()) ++it; //continuo fino a che non torno al trie attuale
        ++it;
        if (it==this->get_leaf().get_children().end()) { //se finiti i children
                    *this=this->get_leaf().get_parent(); //vado al padre e richiamo ++
                    *this=++*this;
        } else { //se ci sono ancora children
            if (it->get_children().isLeaf()) *this=&(*it); //se il child è una foglia, ritorno il puntatore
            else *this=it->begin(); //se il child non è una foglia, ritorno la prima foglia del trie attuale
        }
    } else *this=this->get_leaf().end(); //se non ha il padre
        return *this;
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::leaf_iterator::operator++(int) {
    leaf_iterator tmp = *this;
    if (this->get_leaf().get_parent()) { //se ha il padre
        typename bag<trie<T>>::bag_iterator it = this->get_leaf().get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_leaf().get_label()) ++it; //continuo fino a che non torno al trie attuale
        ++it;
        if (it==this->get_leaf().get_children().end()) { //se finiti i children
                    *this=this->get_leaf().get_parent(); //vado al padre e richiamo ++
                    *this=++*this;
        } else { //se ci sono ancora children
            if (it->get_children().isLeaf()) *this=&(*it); //se il child è una foglia, ritorno il puntatore
            else *this=it->begin(); //se il child non è una foglia, ritorno la prima foglia del trie attuale
        }
    } else *this=this->get_leaf().end(); //se non ha il padre
        return tmp;
}

template <typename T>
bool trie<T>::leaf_iterator::operator==(leaf_iterator const& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::leaf_iterator::operator!=(leaf_iterator const& other) const {
    return m_ptr != other.m_ptr;
}

template <typename T>
trie<T>::leaf_iterator::operator node_iterator() const {
    return node_iterator{m_ptr};
}


//STATIC leaf iterator

template <typename T>
trie<T>::const_leaf_iterator::const_leaf_iterator(trie<T> const* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::begin() const {
    const trie<T>* node = this;
    while (!node->m_c.isLeaf()) {
        node = &(*node->m_c.begin());
    }
    return const_leaf_iterator{node};
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::end() const {
    if (!this->get_parent()) return const_leaf_iterator{nullptr};
    else {
        typename bag<trie<T>>::const_bag_iterator it = this->get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_label()) ++it;
        typename bag<trie<T>>::const_bag_iterator tmp = it;
        ++it;
        if (it==this->get_parent()->get_children().end()) return tmp->get_parent()->end();
        else return const_leaf_iterator{it->begin()};
    }
}

template <typename T>
trie<T> const& trie<T>::const_leaf_iterator::get_leaf() const {
    return *m_ptr;
}

template <typename T>
T const& trie<T>::const_leaf_iterator::operator*() const {
    return *m_ptr->m_l;
}

template <typename T>
T const* trie<T>::const_leaf_iterator::operator->() const {
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::const_leaf_iterator& trie<T>::const_leaf_iterator::operator++() {
    if (this->get_leaf().get_parent()) { //se ha il padre
        typename bag<trie<T>>::const_bag_iterator it = this->get_leaf().get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_leaf().get_label()) ++it; //continuo fino a che non torno al trie attuale
        ++it;
        if (it==this->get_leaf().get_children().end()) { //se finiti i children
                    *this=this->get_leaf().get_parent(); //vado al padre e richiamo ++
                    *this=++*this;
        } else { //se ci sono ancora children
            if (it->get_children().isLeaf()) *this=&(*it); //se il child è una foglia, ritorno il puntatore
            else *this=it->begin(); //se il child non è una foglia, ritorno la prima foglia del trie attuale
        }
    } else *this=this->get_leaf().end(); //se non ha il padre
        return *this;
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::const_leaf_iterator::operator++(int) {
    const_leaf_iterator tmp = *this;
    if (this->get_leaf().get_parent()) { //se ha il padre
        typename bag<trie<T>>::const_bag_iterator it = this->get_leaf().get_parent()->get_children().begin();
        while (*it->get_label()!=*this->get_leaf().get_label()) ++it; //continuo fino a che non torno al trie attuale
        ++it;
        if (it==this->get_leaf().get_children().end()) { //se finiti i children
                    *this=this->get_leaf().get_parent(); //vado al padre e richiamo ++
                    *this=++*this;
        } else { //se ci sono ancora children
            if (it->get_children().isLeaf()) *this=&(*it); //se il child è una foglia, ritorno il puntatore
            else *this=it->begin(); //se il child non è una foglia, ritorno la prima foglia del trie attuale
        }
    } else *this=this->get_leaf().end(); //se non ha il padre
        return tmp;
}

template <typename T>
bool trie<T>::const_leaf_iterator::operator==(const_leaf_iterator const& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::const_leaf_iterator::operator!=(const_leaf_iterator const& other) const {
    return m_ptr != other.m_ptr;
}

template <typename T>
trie<T>::const_leaf_iterator::operator const_node_iterator() const {
    return const_node_iterator{m_ptr};
}
