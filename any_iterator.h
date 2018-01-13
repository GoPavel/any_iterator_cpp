#ifndef ANY_ITERATOR_H
#define ANY_ITERATOR_H
#include <cassert>
#include <exception>
#include <iterator>

// Iterator must be
/*
 * CopyConctructible
 * CopyAssignable
 * lvalue swappable
 */

template<typename T, typename Iterator_tag>
class base_any_iterator {

};

template<typename T, typename Iterator_category_tag>
class any_iterator;

template<typename T>
class base_any_iterator<T, std::random_access_iterator_tag> {
public:
    T& operator[](int32_t dist) {
        return *(static_cast<any_iterator<T, std::random_access_iterator_tag>&>(*this) + dist);
    }
};

class bad_any_iterator: public std::exception {
public:
    char const* message;

    bad_any_iterator(const char *msg) : message(msg) { }

    const char *what() const noexcept override {
        return "bad_any_iterator";
    }
};

static const int SIZE = 16;
static const int ALIGN = 8;
using storage_type = typename std::aligned_storage<SIZE, ALIGN>::type;

template<typename T, typename Iterator_category_tag>
struct meta_information_type;

template<typename T, typename Iterator_category_tag>
class any_iterator: public base_any_iterator<T, Iterator_category_tag> {
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef Iterator_category_tag iterator_category;
    typedef std::ptrdiff_t difference_type;

    storage_type storage;

    typedef meta_information_type<T, Iterator_category_tag> meta_info_type;
    meta_info_type const* ops;

public: ///method
    any_iterator() noexcept
        : ops(meta_info_type::init_empty()) { }

    template <
            typename Iter,
            typename = std::enable_if_t<
                std::is_base_of<
                    Iterator_category_tag,
                    typename std::iterator_traits<Iter>::iterator_category
                    >::value
                >
            >
    constexpr any_iterator(Iter const& other)
        : ops(meta_info_type::template init<Iter>()) {
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
                    Iterator_category_tag,
                    typename std::iterator_traits<Iter>::iterator_category
                    >::value
                >
            >
    constexpr any_iterator(Iter &&other)
        : ops(meta_info_type::template init<Iter>()) {
        if (ops->is_small) {
            new (&storage) Iter(std::move(other));
        } else {
            reinterpret_cast<Iter*&>(storage) = new Iter(std::move(other));
        }
    }

    constexpr any_iterator(any_iterator const& other): ops(other.ops) {
        ops->copy_constr(other.storage, storage);
    }

    constexpr any_iterator(any_iterator &&other) noexcept: ops(other.ops) {
        ops->move_constr(other.storage, storage);
    }

    constexpr any_iterator& operator= (any_iterator const& other) {
        if ((*this) != other) {
            any_iterator temp = other;
            swap(*this, temp);
        }
        return *this;
    }

    constexpr any_iterator& operator= (any_iterator && other) noexcept {
        if ((*this) != other) {
            ops->destroy(storage);
            ops = other.ops;
            ops->move_constr(other.storage, storage);
        }
        return *this;
    }

    ~any_iterator() {
        ops->destroy(storage);
    }

    constexpr T& operator*() const {
        return ops->dereference(storage);
    }

    constexpr bool empty() const noexcept {
        return ops->is_empty;
    }

    template<typename A, typename B>
    friend typename std::enable_if<std::is_base_of<std::forward_iterator_tag, B>::value, any_iterator<A, B>&>::type
    operator++(any_iterator<A, B>&);

    template<typename A, typename B>
    friend typename std::enable_if<std::is_base_of<std::forward_iterator_tag, B>::value, any_iterator<A, B>>::type
    operator++(any_iterator<A, B>&, int);

    constexpr T* operator-> () const{
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

    friend void swap(any_iterator &a, any_iterator &b) noexcept {
        storage_type buf_a, buf_b;
        a.ops->move_constr(a.storage, buf_a);
        b.ops->move_constr(b.storage, buf_b);
        b.ops->move_constr(buf_b, a.storage);
        a.ops->move_constr(buf_a, b.storage);
        std::swap(a.ops, b.ops);
    }

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, B>::value, any_iterator<A, B>&>
    operator--(any_iterator<A, B>&);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, B>::value, any_iterator<A, B>>
    operator--(any_iterator<A, B>&, int);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>&>
    operator+=(any_iterator<A, B>&, int32_t dist);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>&>
    operator-=(any_iterator<A, B>&, int32_t dist);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>>
    operator+(any_iterator<A, B> const &a, int32_t n);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>>
    operator+(int32_t n, any_iterator const &a);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>>
    operator-(any_iterator const &a, int32_t n);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value, any_iterator<A, B>>
    operator-(int32_t n, any_iterator const &a);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value,  bool>
    operator<(any_iterator<A, B> const &a, any_iterator<A, B> const &b);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value,  bool>
    operator>(any_iterator<A, B> const &a, any_iterator<A, B> const &b);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value,  bool>
    operator>=(any_iterator<A, B> const &a, any_iterator<A, B> const &b);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value,  bool>
    operator<=(any_iterator<A, B> const &a, any_iterator<A, B> const &b);

    template<typename A, typename B>
    friend typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, B>::value,  std::ptrdiff_t>
    operator- (any_iterator const &a, any_iterator const &b);
};

template<typename T, typename Iter>
typename std::enable_if<std::is_base_of<std::forward_iterator_tag, Iter>::value, any_iterator<T, Iter>&>::type
operator++(any_iterator<T, Iter> & it) {
    it.ops->inc(it.storage);
    return it;
}

template<typename T, typename Iter>
typename std::enable_if<std::is_base_of<std::forward_iterator_tag, Iter>::value, any_iterator<T, Iter>>::type
operator++(any_iterator<T, Iter> &it, int) {
       any_iterator<T, Iter> temp(it);
       it.ops->inc(it.storage);
       return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, Iter>::value, any_iterator<T, Iter>&>
operator--(any_iterator<T, Iter> &it) {
    it.ops->dec(it.storage);
    return it;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::bidirectional_iterator_tag, Iter>::value, any_iterator<T, Iter>>
operator--(any_iterator<T, Iter> &it, int) {
    any_iterator<T, Iter> temp(it);
    it.ops->dec(it.storage);
    return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>&>
operator+=(any_iterator<T, Iter> &it, int32_t dist) {
    it.ops->add(it.storage, dist);
    return it;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>&>
operator-=(any_iterator<T, Iter> &it, int32_t dist) {
    it.ops->add(it.storage, -dist);
    return it;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>>
operator+(any_iterator<T, Iter> const &a, int32_t n) {
    any_iterator<T, Iter> temp(a);
    temp += n;
    return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>>
operator+(int32_t n, any_iterator<T, Iter> const &a) {
    any_iterator<T, Iter> temp(a);
    temp += n;
    return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>>
operator-(any_iterator<T, Iter> const &a, int32_t n) {
    any_iterator<T, Iter> temp(a);
    temp -= n;
    return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, any_iterator<T, Iter>>
operator-(int32_t n, any_iterator<T, Iter> const &a) {
    any_iterator<T, Iter> temp(a);
    temp -= n;
    return temp;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, bool>
operator<(any_iterator<T, Iter> const &a, any_iterator<T, Iter> const &b) {
    assert(a.ops == b.ops);
    return a.ops->less(a.storage, b.storage);
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, bool>
operator>(any_iterator<T, Iter> const &a, any_iterator<T, Iter> const &b) {
    return b < a;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, bool>
operator>=(any_iterator<T, Iter> const &a, any_iterator<T, Iter> const &b) {
    return a > b || a == b;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, bool>
operator<=(any_iterator<T, Iter> const &a, any_iterator<T, Iter> const &b) {
    return a < b || a == b;
}

template<typename T, typename Iter>
typename std::enable_if_t<std::is_base_of<std::random_access_iterator_tag, Iter>::value, std::ptrdiff_t>
operator-(any_iterator<T, Iter> const &a, any_iterator<T, Iter> const &b) {
    assert(a.ops == b.ops);
    return a.ops->diff(a.storage,  b.storage);
}

template <typename T>
struct meta_information_type<T, std::forward_iterator_tag> {
public:
    bool is_empty;
    bool is_small;
    using copy_constr_type = void (*)(storage_type const&, storage_type &);
    copy_constr_type copy_constr;

    using move_constr_type = void(*)(storage_type &, storage_type &);
    move_constr_type move_constr;

    using destroy_type = void (*)(storage_type &);

    destroy_type destroy;
    using dereference_type = T& (*)(storage_type const &);
    dereference_type dereference;

    using inc_type = void (*)(storage_type &);
    inc_type inc;

    using eq_type = bool(*)(storage_type const &, storage_type const &);
    eq_type eq;

    constexpr meta_information_type(bool is_small,
                                 copy_constr_type copy_constr,
                                 move_constr_type move_constr,
                                 destroy_type destroy,
                                 dereference_type dereference,
                                 inc_type inc,
                                 eq_type eq)
        : is_empty(false),
          is_small(is_small),
          copy_constr(copy_constr),
          move_constr(move_constr),
          destroy(destroy),
          dereference(dereference),
          inc(inc),
          eq(eq)
    { }

    constexpr meta_information_type()
        : is_empty(true),
          is_small(true), // why?
          copy_constr(copy_constr_empty),
          move_constr(move_constr_empty),
          destroy(destroy_empty),
          dereference(dereference_empty),
          inc(inc_empty),
          eq(eq_empty)
    { }

    template<typename Iter>
    static meta_information_type const* init() noexcept {
        static constexpr meta_information_type meta_information_instance  =
                (((sizeof(Iter) <= SIZE) && (alignof(Iter) <= ALIGN) && (std::is_nothrow_move_constructible<Iter>())) ?
                   meta_information_type(
                         true,
                         copy_constr_impl<Iter, true>,
                         move_constr_impl<Iter, true>,
                         destroy_impl<Iter, true>,
                         dereference_impl<Iter, true>,
                         inc_impl<Iter, true>,
                         eq_impl<Iter, true>)
                   : meta_information_type(
                         false,
                         copy_constr_impl<Iter, false>,
                         move_constr_impl<Iter, false>,
                         destroy_impl<Iter, false>,
                         dereference_impl<Iter, false>,
                         inc_impl<Iter, false>,
                         eq_impl<Iter, false>)
                     );
        return &meta_information_instance;
    }

    static meta_information_type const* init_empty() noexcept {
        static constexpr meta_information_type meta_information_instance = meta_information_type();
        return &meta_information_instance;
    }

    template<typename Iter, bool is_small>
    static constexpr void copy_constr_impl(storage_type const &from, storage_type &to) {
        if (is_small)
            new(&to) Iter(reinterpret_cast<Iter const&>(from));
        else
            reinterpret_cast<Iter*&>(to) = new Iter(*reinterpret_cast<Iter const * const&>(from));
    }

    template<typename Iter, bool is_small>
    static constexpr void move_constr_impl(storage_type &from, storage_type &to) noexcept {
        if (is_small) {
            new (&to) Iter(std::move(reinterpret_cast<Iter&>(from)));
        } else {
//            reinterpret_cast<Iter*&>(to) = new Iter(std::move(*reinterpret_cast<Iter*&>(from)));
            reinterpret_cast<Iter *&>(to) = reinterpret_cast<Iter *&>(from);
            reinterpret_cast<Iter *&>(from) = nullptr;
        }
    }

    template<typename Iter, bool is_small>
    static constexpr void destroy_impl(storage_type &data) {
        if (is_small)
            reinterpret_cast<Iter&>(data).Iter::~Iter();
        else {
            if (reinterpret_cast<Iter *&>(data) != nullptr)
                delete reinterpret_cast<Iter*&>(data);
        }
    }

    template<typename Iter, bool is_small>
    static constexpr T& dereference_impl(storage_type const &data) {
        if (is_small)
            return *(reinterpret_cast<Iter const&>(data));
        else {
            assert(reinterpret_cast<Iter * const &>(data) != nullptr);
            return *(*(reinterpret_cast<Iter * const&>(data)));
        }
    }

    template<typename Iter, bool is_small>
    static constexpr void inc_impl(storage_type &data) {
        if (is_small)
            ++(reinterpret_cast<Iter&>(data));
        else {
            assert(reinterpret_cast<Iter *&>(data) != nullptr);
            ++(*(reinterpret_cast<Iter*&>(data)));
        }
    }

    template<typename Iter, bool is_small>
    static constexpr bool eq_impl(storage_type const &a, storage_type const &b) {
        if (is_small)
            return reinterpret_cast<Iter const &>(a) == reinterpret_cast<Iter const &>(b);
        else
            return (*reinterpret_cast<Iter const * const&>(a)) == (*reinterpret_cast<Iter const * const&>(b));
    }

    static constexpr void copy_constr_empty(storage_type const &, storage_type &) noexcept { }

    static constexpr void move_constr_empty(storage_type &, storage_type &) noexcept { }

    static constexpr void destroy_empty(storage_type &) noexcept { }

    static constexpr T& dereference_empty(storage_type const &) { throw bad_any_iterator("dereference empty any_iterator\n"); }

    static constexpr void inc_empty(storage_type &) { throw bad_any_iterator("increment empty any_iterator\n"); }

    static constexpr bool eq_empty(storage_type const &, storage_type const &) noexcept { return true; }
};

template<typename T>
struct meta_information_type<T, std::bidirectional_iterator_tag>
        : public meta_information_type<T, std::forward_iterator_tag> {

    using typename meta_information_type<T, std::forward_iterator_tag>::copy_constr_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::move_constr_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::destroy_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::dereference_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::inc_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::eq_type;

    using dec_type = void(*)(storage_type &);
    dec_type dec;

    constexpr meta_information_type(bool is_small,
                                 copy_constr_type copy_constr,
                                 move_constr_type move_constr,
                                 destroy_type destroy,
                                 dereference_type dereference,
                                 inc_type inc,
                                 eq_type eq,
                                 dec_type dec)
        : meta_information_type<T, std::forward_iterator_tag>(
              is_small,
              copy_constr,
              move_constr,
              destroy,
              dereference,
              inc,
              eq),
          dec(dec)
    { }

    constexpr meta_information_type():
        meta_information_type<T, std::forward_iterator_tag>(),
        dec(dec_empty)
    { }


    template<typename Iter>
    static meta_information_type const *init() noexcept {
        static constexpr meta_information_type meta_information_instance  =
                (((sizeof(Iter) <= SIZE) && (alignof(Iter) <= ALIGN) && (std::is_nothrow_move_constructible<Iter>())) ?
                   meta_information_type(
                         true,
                         meta_information_type<T, std::forward_iterator_tag>::template copy_constr_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template move_constr_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template destroy_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template dereference_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template inc_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template eq_impl<Iter, true>,
                         dec_impl<Iter, true>)
                   : meta_information_type(
                         false,
                         meta_information_type<T, std::forward_iterator_tag>::template copy_constr_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template move_constr_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template destroy_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template dereference_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template inc_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template eq_impl<Iter, false>,
                         dec_impl<Iter, false>)
                     );
        return &meta_information_instance;
    }

    static meta_information_type const* init_empty() noexcept {
        static constexpr meta_information_type meta_information_instance = meta_information_type();
        return &meta_information_instance;
    }

    template<typename Iter, bool is_small>
    static constexpr void dec_impl(storage_type &data) {
        if (is_small)
            --(reinterpret_cast<Iter &>(data));
        else {
            assert(reinterpret_cast<Iter * const &>(data) != nullptr);
            --(*reinterpret_cast<Iter *&>(data));
        }
    }

    static constexpr void dec_empty(storage_type &) { throw bad_any_iterator("decrement empty any iterator\n"); }
};

template<typename T>
struct meta_information_type<T, std::random_access_iterator_tag>
        : public meta_information_type<T, std::bidirectional_iterator_tag> {

    using typename meta_information_type<T, std::forward_iterator_tag>::copy_constr_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::move_constr_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::destroy_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::dereference_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::inc_type;
    using typename meta_information_type<T, std::forward_iterator_tag>::eq_type;
    using typename meta_information_type<T, std::bidirectional_iterator_tag>::dec_type;

    using add_type = void(*)(storage_type &, int32_t const &);
    add_type add;

    using less_type = bool(*)(storage_type const&, storage_type const&);
    less_type less;

    using diff_type = std::ptrdiff_t(*)(storage_type const&, storage_type const&);
    diff_type diff;

    constexpr meta_information_type(bool is_small,
                                 copy_constr_type copy_constr,
                                 move_constr_type move_constr,
                                 destroy_type destroy,
                                 dereference_type dereference,
                                 inc_type inc,
                                 eq_type eq,
                                 dec_type dec,
                                 add_type add,
                                 less_type less,
                                 diff_type diff)
        : meta_information_type<T, std::bidirectional_iterator_tag>(
              is_small,
              copy_constr,
              move_constr,
              destroy,
              dereference,
              inc,
              eq,
              dec),
          add(add),
          less(less),
          diff(diff)
    { }

    constexpr meta_information_type():
        meta_information_type<T, std::bidirectional_iterator_tag>(),
        add(add_empty),
        less(less_empty),
        diff(diff_empty)
    { }


    template<typename Iter>
    static meta_information_type const *init() noexcept {
        static constexpr meta_information_type meta_information_instance  =
                (((sizeof(Iter) <= SIZE) && (alignof(Iter) <= ALIGN) && (std::is_nothrow_move_constructible<Iter>())) ?
                   meta_information_type(
                         true,
                         meta_information_type<T, std::forward_iterator_tag>::template copy_constr_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template move_constr_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template destroy_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template dereference_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template inc_impl<Iter, true>,
                         meta_information_type<T, std::forward_iterator_tag>::template eq_impl<Iter, true>,
                         meta_information_type<T, std::bidirectional_iterator_tag>::template dec_impl<Iter, true>,
                         add_impl<Iter, true>,
                         less_impl<Iter, true>,
                         diff_impl<Iter, true>)
                   : meta_information_type(
                         false,
                         meta_information_type<T, std::forward_iterator_tag>::template copy_constr_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template move_constr_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template destroy_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template dereference_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template inc_impl<Iter, false>,
                         meta_information_type<T, std::forward_iterator_tag>::template eq_impl<Iter, false>,
                         meta_information_type<T, std::bidirectional_iterator_tag>::template dec_impl<Iter, false>,
                         add_impl<Iter, false>,
                         less_impl<Iter, false>,
                         diff_impl<Iter, false>)
                     );
        return &meta_information_instance;
    }

    static meta_information_type const* init_empty() noexcept {
        static constexpr meta_information_type meta_information_instance = meta_information_type();
        return &meta_information_instance;
    }

    template<typename Iter, bool is_small>
    static constexpr void add_impl(storage_type &data, int32_t const &dist) {
        if(is_small)
            (reinterpret_cast<Iter &>(data)) += dist;
        else {
            assert(reinterpret_cast<Iter * const &>(data) != nullptr);
            (*reinterpret_cast<Iter *&>(data)) += dist;
        }
    }

    template<typename Iter, bool is_small>
    static constexpr bool less_impl(storage_type const &a, storage_type const &b) {
        if(is_small)
            return (reinterpret_cast<Iter const&>(a)) < (reinterpret_cast<Iter const&>(b));
        else
            return (*reinterpret_cast<Iter const * const&>(a)) < (*reinterpret_cast<Iter const* const&>(b));

    }

    template<typename Iter, bool is_small>
    static constexpr std::ptrdiff_t diff_impl(storage_type const &a, storage_type const &b) {
        if (is_small)
            return reinterpret_cast<Iter const&>(a) - reinterpret_cast<Iter const&>(b);
        else {
            assert(reinterpret_cast<Iter const * const&>(a) != nullptr);
            assert(reinterpret_cast<Iter const * const&>(b) != nullptr);
            return reinterpret_cast<Iter const * const&>(a)  - reinterpret_cast<Iter const * const &>(b);
        }
    }

    static constexpr void add_empty(storage_type &, int32_t const&) { throw bad_any_iterator("increment empty any iterator\n"); }

    static constexpr bool less_empty(storage_type const &, storage_type const &) { throw bad_any_iterator("less for empty any iterator\n"); }

    static constexpr std::ptrdiff_t diff_empty(storage_type const &, storage_type const &) {throw bad_any_iterator("diff for empty any iterator]n"); }
};

template <typename T>
using any_forward_iterator = any_iterator<T, std::forward_iterator_tag>;

template <typename T>
using any_bidirectional_iterator = any_iterator<T, std::bidirectional_iterator_tag>;

template <typename T>
using any_random_access_iterator = any_iterator<T, std::random_access_iterator_tag>;

#endif//ANY_ITERATOR_H
