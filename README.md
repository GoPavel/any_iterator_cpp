# any_iterator_cpp
Implementation type erasure on C++ for iterator.

## Points of implementation

- **alignment safety**

`any_iterator`'s size and align must be same for different erased iterators. Therefore `std::aligned_storage<SIZE, ALIGN>` is used.

Note: `SIZE=16`, `ALIGN=8`

- **exception safety**

- implements **STL-inteface** for interator [link](https://en.cppreference.com/w/cpp/named_req/Iterator)

If your iterator is [bidirectional](https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator) then `any_iterator` from that is bidirectional.

- **small-object optimization**

If iterator's size less than `SIZE` and iterator's align less than `ALIGN` and iterator is nothow move constructible then `any_iterator` doesn't use heap.

Note: `any_iterator` always have noexcept `swap` what causes the last requirement.

- saves ability to **explicit instantiation**.

Member-method `operator[]` shouldn't be contained in the `any_iterator<T, std::forward_iterator_tag>`. SFINAE in that case brokes explicit instatiation. (In other cases, conditional operators are just friends.) Therefore CRTP-like style is [used](https://github.com/GoPavel/any_iterator_cpp/blob/master/any_iterator.h#L49).

- **shared fuction tables**

Two `any_iterator`s with same original type have a pointer to same [table of method](https://github.com/GoPavel/any_iterator_cpp/blob/master/any_iterator.h#L333).

## Example
```c++
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
```


## Build and run test

```bash
$ cmake .
$ make test
$ ./test
```

## Build and run [example](https://github.com/GoPavel/any_iterator_cpp/blob/master/example.cpp)

```bash
$ cmake .
$ make example
$ ./example
```
