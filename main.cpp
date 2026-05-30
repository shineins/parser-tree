#include "trie.cpp"
#include <fstream>
int main()
{/*std::vector<char>s={'a','b','c'};
    std::ifstream ifs("C:\\Users\\agnod\\Desktop\\900792\\foglio.txt");
    std::ifstream is("C:\\Users\\agnod\\Desktop\\900792\\foglio2.txt");
    trie<char> t;
    ifs >> t;
    trie<char> t2;
    is >> t2;
    const trie<char> tt = t;
    trie<char> sum;*/
    //t2.path_compress();
    //std::cout<<t2;

    std::ifstream ifs("sheet3.txt");
    trie<int> t;
    ifs >> t;
    std::cout<<t<<std::endl;
    std::vector<int>s{5,21,12};
    //std::cout<<*t[s].get_parent();
    t.path_compress();
    //std::cout<<t<<std::endl;
    std::cout<<*t[s].get_parent();
    for (auto node_it = t[s].root();node_it!=t.root();++node_it)
    std::cout<<*node_it<<std::endl;

    for (auto leaf_it = t.begin();leaf_it!=t.end();++leaf_it)
    std::cout<<*leaf_it<<std::endl;

    //std::cout<<*t[s].get_parent();
    //std::cout<<t<<std::endl;
    /*sum = t + t2;std::cout<<sum<<std::endl;
    for (typename trie<char>::node_iterator node_it = sum[s].root();node_it!=sum.root();++node_it) std::cout<<*node_it;*/
    //t+=t2;
    //std::cout<<*sum[s].get_parent()<<std::endl;
    //std::cout<<sum.max()<<std::endl;
    //std::cout<< t2<<std::endl;
    //std::cout << sum;
    return 0;
}
