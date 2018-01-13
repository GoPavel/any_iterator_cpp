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

template<typename Iter>
void Test_Forward() {
    cout << "Forward_iterator test" << endl;
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
        IterF any_it(it);
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
        for (Iter it = vec.begin(); it != vec.end(); ++it, ++j) {
            assert(j == *it);
        }
        {
            std::list<int> list;
            for(int i = 0; i < 10; ++i) {
                vec.push_back(i);
            }
            int j = 0;
            for (IterF it = list.begin(); it != list.end(); ++it, ++j) {
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
        cout << "test 5" << endl;
        int a[5] = {1, 2, 3, 4, 5};
        int *p = a;
        IterF it = p;
        for (int i = 1; i < 6; ++i) {
            assert(*(it++) == i);
        }
    }
    cout << endl;
}

template<typename Iter>
void Test_Bidirect() {
    cout << "Bidirect iterator test" << endl;
    cout << "base: ";
    Test_Forward<Iter>();
    {
        cout << "test1" << endl;
        std::vector<int> v;
        for (int i = 0; i < 10; ++i) {
            v.push_back(i);
        }
        int i = 0;
        for (Iter it = v.begin(); it != v.end(); ++it, ++i) {
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
        for (Iter it = v.end(); it != Iter(v.begin());) {
            --it;
            --i;
            assert(*it == i);
        }
    }
   {
        cout << "test3" << endl;
        std::deque<int> deq;
        for (int i = 0; i < 10; ++i) {
            deq.push_back(i);
        }
        int i = 0;
        for (Iter it = deq.begin(); it != deq.end(); it++, ++i) {
            assert(*it == i);
        }
    }
    {
        cout << "test4" << endl;
        std::deque<int> deq;
        for (int i = 0; i < 10; ++i) {
            deq.push_back(i);
        }
        int i = 10;
        for (Iter it = deq.end(); it != deq.begin();) {
            --it;
            --i;
            assert(*it == i);
        }
    }
    {
        cout << "test5" << endl;
        std::deque<int> d;
        std::vector<int> v;
        for (int i = 0; i < 10; ++i) {
            d.push_back(i);
            v.push_back(i);
        }
        Iter a = d.begin();
        Iter b = v.begin();
        assert(!(a == b));
        assert(a != b);
        for(int i = 0; i < 10; ++i) {
            assert(*a == *b);
            ++a;
            swap(a, b);
            ++a;
        }
    }
    cout << "end Bidirect iterator test" << endl;;
}

template<typename Iter>
void Test_Random() {
    cout << "Random_iterator test" << endl;
    cout << "base: ";
    Test_Bidirect<any_iterator<int, std::random_access_iterator_tag>>();
    {
        cout << "test1" << endl;
        std::vector<int> v;
        for (int i = 0; i < 10; ++i) {
            v.push_back(i);
        }
        Iter it = v.begin();
        for (int i = 0; i < 10; ++i) {
//            cout << it[i];
            assert(it[i] == i);
        }
    }
    {
        cout << "test2" << endl;
        std::vector<int> v;
        for (int i = 0; i < 100; ++i) {
            v.push_back(i);
        }
        Iter it = v.end();
        int i = 100;
        for (int j = 1; (i - j) >= 0; ++j) {
            i -= j;
            it -= j;
            assert(i == it[0]);
        }
    }
    {
        cout << "test3" << endl;
        std::deque<int> d;
        for (int i = 0; i < 10; ++i) {
            d.push_back(i);
        }
        Iter it = d.begin();
        for (int i = 0; i < 10; ++i) {
//            cout << it[i];
            assert(it[i] == i);
        }
    }
    {
        cout << "test4" << endl;
        std::deque<int> d;
        for (int i = 0; i < 100; ++i) {
            d.push_back(i);
        }
        Iter it = d.end();
        int i = 100;
        for (int j = 1; (i - j) >= 0; ++j) {
            i -= j;
            it -= j;
            assert(i == it[0]);
        }
    }
    {
        cout << "test5" << endl;
        std::vector<int> v;
        std::deque<int> d;
        for(int i = 0; i < 10; ++i) {
            v.push_back(i);
            d.push_back(i);
        }
        Iter a = v.begin();
        Iter b = a;
        assert(a <= b);
        assert(b <= a);
        assert(a <= a);
        assert(b <= b);
        ++b;
        for(int i = 1; i < 10; ++i) {
            assert(a < b);
            assert(!(b < a));
            ++b;
        }
    }
    cout << "end Random_iterator test" << endl;
}

void usefull_info() {
    cout << "Info: " << endl;
    {
        cout << "iter small or not for int\n";
        cout << "vector::iterator" << std::boolalpha << " "
             << small_info<std::vector<int>::iterator, int>() << endl;
        cout << "list::iterator" << std::boolalpha << " "
             << small_info<std::list<int>::iterator, int>() << endl;
        cout << "deque::iterator" << std::boolalpha << " "
             << small_info<std::deque<int>::iterator, int>() << endl;
    }
    cout << endl;
}

void Test_const() {
    cout << "test const iterator" << endl;
    typedef any_iterator<const int, std::forward_iterator_tag> const_iter;
    std::vector<int> v;
    for (int i = 0; i < 10; ++i) { v.push_back(i); }
    cout << "end test const iterator" << endl;
    int i = 0;
    for (const_iter it = v.begin(); it != v.end(); ++it, ++i) {
        assert(*it == i);
    }
    std::set<int> s;
    for (int i = 0; i < 20; ++i) {
        s.insert(i);
    }
    i = 0;
    for (const_iter it = s.begin(); it != s.end(); ++it, ++i) {
        assert(*it ==  i);
//        cout << *it;
    }
}

int main() {
    Test_Forward<any_iterator<int, std::forward_iterator_tag>>();
    Test_Bidirect<any_iterator<int, std::bidirectional_iterator_tag>>();
    Test_Random<any_iterator<int, std::random_access_iterator_tag>>();
    Test_const();
    return 0;
}
