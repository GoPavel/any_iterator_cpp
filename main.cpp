#include <iostream>
#include "any_iterator.h"
#include <vector>
#include <type_traits>
#include <iterator>
#include <list>
#include <set>
#include <deque>
#include <typeinfo>

//template class any_iterator<int, std::forward_iterator_tag>;

typedef any_iterator<int, std::forward_iterator_tag> IterF;
typedef any_iterator<int, std::bidirectional_iterator_tag> IterB;
typedef any_iterator<int, std::random_access_iterator_tag> IterR;

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
    cout << "forward_test" << endl;
    {
        cout << "test 1" << endl;
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        std::vector<int>::iterator it = vec.begin();
        IterF any_it(it);
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
        IterF any_it(it);
        assert(*it == *any_it); // 1
        it++;
        any_it++;
        assert(*it == *any_it); // 2
        it++;
        any_it++;
        assert(*it == *any_it); // end
        IterF b;
        b = deq.begin();

    }
    {
        cout << "test 2" << endl;
        std::vector<int> vec;
        for(int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        int j = 0;
        for (IterF it = vec.begin(); it != IterF(vec.end()); ++it, ++j) {
            assert(j == *it);
        }
        {
            std::list<int> list;
            for(int i = 0; i < 10; ++i) {
                vec.push_back(i);
            }
            int j = 0;
            for (IterF it = list.begin(); it != IterF(list.end()); ++it, ++j) {
                assert(j == *it);
            }
        }
    }
    {
        cout << "test 3" << endl;
        std::vector<int> vec;
        vec.push_back(666);
        vec.push_back(777);
        IterF a;
        assert(a.empty());
        a = vec.begin();
        assert(!a.empty());
        a = IterF();
        assert(a.empty());
        a = vec.begin();
        IterF b = a;
        b++;
        assert((*b) - (*a) == 111);
        swap(a, b);
        assert((*b) - (*a) == -111);
    }
    {
        cout << "test 4" << endl;
        int const cnt = 10;
        std::vector<int> v;
        std::list<int> l;
        std::deque<int> q;
        for (int i = 0; i < cnt; ++i) {
            v.push_back(i);
            l.push_back(i);
            q.push_back(i);
        }
        IterF its[3];
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
//    {
//        cout << "test fail 1" << endl;
//        std::vector<int> v;
//        v.push_back(5);
//        Iter it = v.begin();
//        --it;
//    }
    {
        cout << "test5" << endl;
        int a[5] = {1, 2, 3, 4, 5};
        int *p = a;
        IterF it = p;
        for (int i = 1; i < 6; ++i) {
            assert(*(it++) == i);
        }
    }
    cout << "bidirect_test" << endl;
    {
        cout << "test1" << endl;
        std::vector<int> v;
        for (int i = 0; i < 10; ++i) {
            v.push_back(i);
        }
        int i = 0;
        for (IterB it = v.begin(); it != IterB(v.end()); ++it, ++i) {
            assert(*it == i);
        }
    }
    {
        cout << "test2" << endl;
        std::vector<int> v;
        for (int i = 0; i < 10; ++i) {
            v.push_back(i);
        }
        int i = 10;
        for (IterB it = v.end(); it != IterB(v.begin());) {
            --it;
            --i;
            assert(*it == i);
        }
    }
    {
       {
            cout << "test3" << endl;
            std::deque<int> deq;
//            for (int i = 0; i < 0; ++i) {
//                deq.push_back(i);
//            }
//            int i = 0;
            assert(deq.begin() == deq.end());
            assert(IterB(deq.begin()) == IterB(deq.end()));
//            for (IterF it = deq.begin(); it != IterF(deq.end()); it++, ++i) {
//                assert(*it == i);
//                cout << *it;
//            }
        }
//        {
//            cout << "test2" << endl;
//            std::deque<int> deq;
//            for (int i = 0; i < 10; ++i) {
//                deq.push_back(i);
//            }
//            int i = 10;
//            for (IterB it = deq.end(); it != IterB(deq.begin());) {
//                --it;
//                --i;
//                assert(*it == i);
//            }
//        }

    }
    return 0;
}


// TODO const iter ?
