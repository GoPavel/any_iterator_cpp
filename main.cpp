#include <iostream>
#include "any_iterator.h"
#include <vector>
#include <type_traits>
#include <iterator>
#include <list>
#include <set>
#include <deque>
#include <typeinfo>

template class any_iterator<int, std::forward_iterator_tag>;

typedef any_iterator<int, std::forward_iterator_tag> Iter;

using std::endl;
using std::cout;

template<typename Iter, typename T>
bool small_info() {
    return   (sizeof(Iter) <= 16) &&
             (alignof(Iter) <= 8) &&
             (std::is_nothrow_move_constructible<Iter>());
}

int main() {
    {
        cout << "iter small or not for int\n";
        cout << "vector::iterator" << std::boolalpha << " "
             << small_info<std::vector<int>::iterator, int>() << endl;
        cout << "list::iterator" << std::boolalpha << " "
             << small_info<std::list<int>::iterator, int>() << endl;
        cout << "deque::iterator" << std::boolalpha << " "
             << small_info<std::deque<int>::iterator, int>() << endl;
    }
    {
        cout << "test 1" << endl;
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        std::vector<int>::iterator it = vec.begin();
        Iter any_it(it);
        assert(*it == *any_it); // 1
        it++;
        any_it++;
        assert(*it == *any_it); // 2
        it++;
        any_it++;
        assert(*it == *any_it); // 3
        it++;
        any_it++;
        assert(*it == *any_it); // endl
    }
    {
        cout << "test 1.5" << endl;
        std::deque<int> deq;
        deq.push_back(1);
        deq.push_back(2);

        std::deque<int>::iterator it = deq.begin();
        Iter any_it(it);
        assert(*it == *any_it); // 1
        it++;
        any_it++;
        assert(*it == *any_it); // 2
        it++;
        any_it++;
        assert(*it == *any_it); // end
        Iter b;
        b = deq.begin();

    }
    {
        cout << "test 2" << endl;
        std::vector<int> vec;
        for(int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        int j = 0;
        for (Iter it = vec.begin(); it != Iter(vec.end()); ++it, ++j) {
            assert(j == *it);
        }
        {
            std::list<int> list;
            for(int i = 0; i < 10; ++i) {
                vec.push_back(i);
            }
            int j = 0;
            for (Iter it = list.begin(); it != Iter(list.end()); ++it, ++j) {
                assert(j == *it);
            }
        }
    }
    {
        cout << "test 3" << endl;
        std::vector<int> vec;
        vec.push_back(666);
        vec.push_back(777);
        Iter a;
        assert(a.empty());
        a = vec.begin();
        assert(!a.empty());
        a = Iter();
        assert(a.empty());
        a = vec.begin();
        Iter b = a;
        b++;
        assert((*b) - (*a) == 111);
        swap(a, b);
        assert((*b) - (*a) == -111);
    }
    {
        cout << "test 4" << endl;
        int const cnt = 0;
        std::vector<int> v;
        std::list<int> l;
        std::deque<int> q;
        for (int i = 0; i < cnt; ++i) {
            v.push_back(i);
            l.push_back(i);
            q.push_back(i);
        }
        Iter its[3];
        its[0] = v.begin();
        its[1] = l.begin();
        its[2] = q.begin();
        for (int i = 0; i < cnt; ++i) {
            for (int j = 0; j < 3; ++j) {
                assert(*its[j] == i);
                its[j]++;
            }
        }
    }
    return 0;
}


// TODO const iter ?
