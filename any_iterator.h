#ifndef ANY_ITERATOR_H
#define ANY_ITERATOR_H

#include <type_traits>
#include <iterator>
#include <cassert>

template <typename T, typename iterator_categories_tag>
class any_iterator;

template<typename T>
class any_iterator<T, std::forward_iterator_tag> {
    typedef  std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;

    static const int SIZE = 16;
    static const int ALLIGN = 8;
    using storage_t = typename std::aligned_storage<SIZE, ALLIGN>::type;

    storage_type storage;

    struct meta_information_t {
        size_t align;
        bool is_nothrow_move;
        using dereference_t = reference (*)();
        dereference_t dereference;

        using inc_t = reference (*)();
        inc_t inc;
        void (*destroy)(storage_t &);
        void (*move)(storage_t &, storage_t &);
        constexpr meta_information_t(size_t align,
                                     bool is_nothrow_move,
                                     dereference_t dereference,
                                     inc_t inc)
            : align(align),
              is_nothrow_move(is_nothrow_move),
              dereference(dereference),
              inc(inc){ } //TODO

        constexpr meta_information_t()
            : align(8),
              is_nothrow_move(true) { } //TODO
    }* meta_information;

    static constexpr bool is_small_type() {
        return     (sizeof(T) <= SIZE)
                && (meta_information->align <= ALLIGN)
                && (meta_information->is_nothrow_move);
    }

    template<typename Some_iterator>
    static constexpr meta_information_t* init_meta_information() {
        static meta_information_t meta_information_instance  =
                ((sizeof(T) <= SIZE) && (alignof(Some_iterator) <= ALLIGN) && (std::is_nothrow_move_constructible<Some_iterator>()) ?
                   meta_information_t(
                         alignof(Some_iterator),
                         std::is_nothrow_move_constructible<Some_iterator>(),
                         get_dereference_small<T, Some_iterator>,
                         get_inc_small<T, Some_iterator>) :
                   meta_information_t(
                         alignof(Some_iterator),
                         std::is_nothrow_move_constructible<Some_iterator>(),
                         get_dereference_big<T, Some_iterator>,
                         get_inc_big<T, Some_iterator>)
                     );
        return meta_information_instance;
    } //TODO

    template<typename T, typename Some_iterator>
    static constexpr T& get_dereference_small() {
        return *(reinterpret_cast<Some_iterator&>(storage));
    }

    template<typename T, typename Some_iterator>
    static constexpr T& get_dereference_big() {
        return *(*(reinterpret_cast<Some_iterator*>(storage)));
    }

    template<typename T, typename Some_iterator>
    static constexpr T& get_inc_small() {
        return ++(reinterpret_cast<Some_iterator&>(storage));
    }

    template<typename T, typename Some_iterator>
    static constexpr T& get_inc_big() {
        return ++(*(reinterpret_cast<Some_iterator*>(storage)));
    }


public: ///method
    any_iterator() noexcept
        : meta_information() { }

    template <
            typename Some_iterator,
            typename std::enable_if_t<
                std::is_base_of<
                    iterator_category,
                    typename std::iterator_traits<Some_iterator>::iterator_category
                    >::value
                >
            >
    any_iterator(Some_iterator const& other)
        : meta_information(init_meta_information<T, Some_iterator>()) {
        //TODO
    }

    T& operator*() {
        return meta_information->dereference(storage);
    }

    T& operator++() {
        return meta_information->inc();
    }

    T operator++(int) {

    }

    //operator=
    //operator++ pre and post
    //operator*()
    //operator->
    //operator==
    //operator!=
}

#endif // ANY_ITERATOR_H
