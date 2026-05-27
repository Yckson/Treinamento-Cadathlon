#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>
#include <unordered_map>


using namespace std;

namespace OBDD {

    enum Values{
        NODE_FALSE = 0,
        NODE_TRUE = 1,
        NOT_SET = 2,
        INITIAL_NODE = 3 
    };



    struct OBDD_node {
        size_t id;
        size_t low;
        size_t high;
        string name = string ();

        bool operator== (const OBDD_node& node) const {
            return this->id == node.id && this->low == node.low && this->high == node.high && this->name == node.name;
        }

    };

    using OBDD_tree = vector<OBDD_node>;

}

namespace std {
    template <>
    struct hash <OBDD::OBDD_node> {

        size_t operator () (const OBDD::OBDD_node node) const {
            hash<uint32_t> hasher;
            size_t seed = hasher(node.id);
            seed ^= hasher(node.low)  + 0x9E3779B9 + (seed << 6) + (seed >> 2);
            seed ^= hasher(node.high) + 0x9E3779B9 + (seed << 6) + (seed >> 2);

            return seed;
        }


    };
}

OBDD::OBDD_tree tree;
unordered_map<OBDD::OBDD_node, size_t> unique;
vector<string> valNames;
vector<uint8_t> truthTableR; 

void createTree (int32_t nVal){

    using namespace OBDD;

    tree.push_back({tree.size(), })






}




int32_t main () {

    using namespace OBDD;

    
    tree.reserve(INITIAL_NODE);
    

    int32_t qtd; cin >> qtd;

    
    valNames.reserve(qtd);
    truthTableR.reserve(qtd*qtd);

    for (int i = 0; i < qtd; i++){
        string valName; cin >> valName; valNames[i] = valName;    
    }

    for (int32_t i = 0; i < qtd*qtd; i++){
        uint8_t val;
        for (int32_t j = 0; j < qtd; j++){
            
            cin >> val;
        }

        cin >> val;
        truthTableR[i] = val;
    }






    return 0;
}