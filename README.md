# any_iterator_cpp
Implementation type erasure on C++ for iterator.

Point of implementation:
- save alignment
- exception safety
- implement STL-inteface for interator [link](https://en.cppreference.com/w/cpp/named_req/Iterator)
- small-object optimization, i.e small iterator store on stack.
- save ability to explicit instantiation, that's harder than it seems. So used CRTP for conditional delete `operator[]`.

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
