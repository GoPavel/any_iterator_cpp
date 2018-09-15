#include "any_iterator.h"
#include <vector>
#include <list>
#include <iostream>

using Iter = any_iterator<int, std::bidirectional_iterator_tag>;

int main() {
    std::vector<int> v = {1, 2, 3};
    std::list<int> l = {3, 4, 5};

    std::pair<Iter, Iter> iters[] = {std::make_pair(v.begin(), v.end()), std::make_pair(l.begin(), l.end())};
    for(int i = 0; i < 2; ++i) {
        for(auto it = iters[i]; it.first != it.second; ++it.first) {
            std::cout << *(it.first) << " ";
        }
    }
    std::cout << std::endl;

    return 0;
}
