#ifndef ANY_ITERATOR_H
#define ANY_ITERATOR_H

#include <type_traits>
#include <iterator>
#include <cassert>

// Iterator must be
/*
 * CopyConctructible
 * CopyAssignable
 * lvalue swappable
 */

class bad_any_iterator: public std::exception {
public:
    char const* message;

    bad_any_iterator(const char *msg) : message(msg) { }

    const char *what() const noexcept override {
        return "bad_any_iterator";
    }
};

template <typename T, typename iterator_categories_tag>
class any_iterator;

template<typename T>
class any_iterator<T, std::forward_iterator_tag> {
    typedef std::forward_iterator_tag iterator_category;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;

    static const int SIZE = 16;
    static const int ALIGN = 8;
    using storage_type = typename std::aligned_storage<SIZE, ALIGN>::type;

    storage_type storage;

    struct meta_information_type;
    meta_information_type const* ops;

public: ///method
    any_iterator() noexcept
        : ops(meta_information_type::init_empty()) { }

    template <
            typename Iter,
            typename = std::enable_if_t<
                std::is_base_of<
                    iterator_category,
                    typename std::iterator_traits<Iter>::iterator_category
                    >::value
                >
            >
    any_iterator(Iter const& other)
        : ops(meta_information_type::template init<Iter>()) {
        if(ops->is_small) {
            new (&storage) Iter(other);
        } else {
            reinterpret_cast<Iter*&>(storage) = new Iter(other);
        }
    }

    template <
            typename Iter,
            typename = std::enable_if_t<
                std::is_base_of<
                    iterator_category,
                    typename std::iterator_traits<Iter>::iterator_category
                    >::value
                >
            >
    any_iterator(Iter &&other)
        : ops(meta_information_type::template init<Iter>()) {
        if (ops->is_small) {
            new (&storage) Iter(std::move(other));
        } else {
            reinterpret_cast<Iter*&>(storage) = new Iter(std::move(other));
        }
    }

    any_iterator(any_iterator const& other): ops(other.ops) {
        ops->copy(other.storage, storage);
    }

    any_iterator(any_iterator &&other): ops(other.ops) {
        ops->move(other.storage, storage);
    }

    any_iterator& operator= (any_iterator const& other) {
        if ((*this) != other) {
            ops->destroy(storage);
            ops = other.ops;
            ops->copy(other.storage, storage);
        }
        return *this;
    }

    any_iterator& operator= (any_iterator && other) {
        if ((*this) != other) {
            ops->destroy(storage);
            ops = other.ops;
            ops->move(other.storage, storage);
        }
        return *this;
    }

    ~any_iterator() {
        ops->destroy(storage);
    }

    T& operator*() const {
        return ops->dereference(storage);
    }

    bool constexpr empty() const noexcept {
        return ops->is_empty;
    }

    any_iterator operator++() {
        ops->inc(storage);
        return *this;
    }

    any_iterator operator++(int) {
        any_iterator temp(*this);
        ops->inc(storage);
        return temp;
    }

    T* operator-> () {
        return &ops->dereference(storage);
    }

    friend bool operator== (any_iterator const &a, any_iterator const &b) {
        if (a.ops == b.ops) {
            return a.ops->eq(a.storage, b.storage);
        } else
            return false;
    }

    friend bool operator!= (any_iterator const &a, any_iterator const &b) {
        return !(a == b);
    }

    friend void swap(any_iterator &a, any_iterator &b) {
        storage_type buf_a, buf_b;
        a.ops->move(a.storage, buf_a);
        b.ops->move(b.storage, buf_b);
        b.ops->move(buf_b, a.storage);
        a.ops->move(buf_a, b.storage);
        std::swap(a.ops, b.ops);
    }
};

template <typename T>
struct any_iterator<T, std::forward_iterator_tag>::meta_information_type {
public:
    bool is_empty;
    bool is_small;
    using dereference_type = T& (*)(storage_type const &);
    dereference_type dereference;

    using inc_type = void (*)(storage_type &);
    inc_type inc;

    using destroy_type = void (*)(storage_type &);
    destroy_type destroy;

    using copy_constr_type = void (*)(storage_type const&, storage_type &);
    copy_constr_type copy_constr;

    using copy_assign_type = void (*)(storage_type const&, storage_type &);
    copy_assign_type copy_assign;

    using move_constr_type = void(*)(storage_type &, storage_type &);
    move_constr_type move_constr;

    using move_assign_type = void(*)(storage_type &, storage_type &);
    move_assign_type move_assign;

    using eq_type = bool(*)(storage_type const &, storage_type const &);
    eq_type eq;

    constexpr meta_information_type(bool is_small,
                                 copy_constr_type copy_constr,
                                 copy_assign_type copy_assign,
                                 move_constr_type move_constr,
                                 move_assign_type move_assign,
                                 dereference_type dereference,
                                 inc_type inc,
                                 destroy_type destroy,
                                 eq_type eq)
        : is_empty(false),
          is_small(is_small),
          dereference(dereference),
          inc(inc),
          destroy(destroy),
          copy(copy),
          move(move),
          eq(eq)
    { }

    constexpr meta_information_type()
        : is_empty(true),
          is_small(true),
          dereference(dereference_empty),
          inc(inc_empty),
          destroy(destroy_empty),
          copy(copy_empty),
          move(move_empty),
          eq(eq_empty)
    { }

    template<typename Iter>
    static meta_information_type const* init() noexcept {
        static constexpr meta_information_type meta_information_instance  =
                (((sizeof(Iter) <= SIZE) && (alignof(Iter) <= ALIGN) && (std::is_nothrow_move_constructible<Iter>())) ?
                   meta_information_type(
                         true,
                         dereference_impl<Iter, true>,
                         inc_impl<Iter, true>,
                         destroy_impl<Iter, true>,
                         copy_impl<Iter, true>,
                         move_impl<Iter, true>,
                         eq_impl<Iter, true>)
                   : meta_information_type(
                         false,
                         dereference_impl<Iter, false>,
                         inc_impl<Iter, false>,
                         destroy_impl<Iter, false>,
                         copy_impl<Iter, false>,
                         move_impl<Iter, false>,
                         eq_impl<Iter, false>)
                     );
        return &meta_information_instance;
    }

    static meta_information_type const* init_empty() noexcept {
        static constexpr meta_information_type meta_information_instance = meta_information_type();
        return &meta_information_instance;
    }

    template<typename Iter, bool is_small>
    static constexpr T& dereference_impl(storage_type const &storage) {
        if (is_small)
            return *(reinterpret_cast<Iter const&>(storage));
        else
            return *(*(reinterpret_cast<Iter * const&>(storage)));
    }

    template<typename Iter, bool is_small>
    static constexpr void inc_impl(storage_type &storage) {
        if (is_small)
            ++(reinterpret_cast<Iter&>(storage));
        else
            ++(*(reinterpret_cast<Iter*&>(storage)));
    }

    template<typename Iter, bool is_small>
    static constexpr void destroy_impl(storage_type &storage) {
        if (is_small)
            reinterpret_cast<Iter&>(storage).Iter::~Iter();
        else
            delete reinterpret_cast<Iter*&>(storage);
    }

    template<typename Iter, bool is_small>
    static constexpr void copy_impl(storage_type const &from, storage_type &to) {
        if (is_small)
            new(&to) Iter(reinterpret_cast<Iter const&>(from));
        else
            reinterpret_cast<Iter*&>(to) = new Iter(*reinterpret_cast<Iter const * const&>(from));
    }

    template<typename Iter, bool is_small>
    static constexpr void move_impl(storage_type &from, storage_type &to) {
        if (is_small) {
            new (&to) Iter(std::move(reinterpret_cast<Iter&>(from)));
        } else {
            reinterpret_cast<Iter*&>(to) = new Iter(std::move(*reinterpret_cast<Iter*&>(from)));
        }
    }

    template<typename Iter, bool is_small>
    static constexpr bool eq_impl(storage_type const &a, storage_type const &b) {
        if (is_small)
            return reinterpret_cast<Iter const &>(a) == reinterpret_cast<Iter const &>(b);
        else
            return (reinterpret_cast<Iter const * const&>(a)) == (reinterpret_cast<Iter const * const&>(b));
    }

    static constexpr T& dereference_empty(storage_type const &) { throw bad_any_iterator("dereference empty any_iterator\n"); }

    static constexpr void inc_empty(storage_type &) { throw bad_any_iterator("increment empty any_iterator\n"); }

    static constexpr void destroy_empty(storage_type &) noexcept { }

    static constexpr void copy_empty(storage_type const &, storage_type &) noexcept { }

    static constexpr void move_empty(storage_type &, storage_type &) { }

    static constexpr bool eq_empty(storage_type const &, storage_type const &) noexcept { return true; }
};
#endif // ANY_ITERATOR_H
