//
// Created by bokket on 2024/2/21.
//

#pragma once

#include <memory>

namespace bokket
{

struct ForwardIteratorTag {};
struct BidirectionalIteratorTag:public  ForwardIteratorTag{};
struct RandomAccessIteratorTag:public BidirectionalIteratorTag{};


template<class D,class Tag,bool Const>
class IteratorFacadeNoValueType
{
public:

    friend bool operator==(D const& lhs,D const& rhs) {
        return equal(lhs,rhs);
    }

    friend bool operator!=(D const& lhs,D const& rhs) {
        return !(lhs==rhs);
    }

    D& operator++() {
        derived().increment();
        return derived();
    }

    D operator++(int) {
        auto ret = derived(); // copy
        derived().increment();
        return ret;
    }

    D& operator--() {
        derived().decrement();
        return derived();
    }

    D operator--(int) {
        // D tmp(this->derived());
        auto ret = derived(); // copy
        derived().decrement();
        return ret;
    }

protected:
    D &derived() {
        return static_cast<D&>(*this);
    }

    D const &derivedConst() const {
        return static_cast<D const&>(*this);
    }

    static bool equal(D const& lhs,D const& rhs) {
        return lhs.equal(rhs);
    }
};


template<class T>
struct RemoveConst
{
    typedef T type;
};

template<class T>
struct RemoveConst<const T>
{
    typedef T type;
};

template<bool B,class T,class F>
struct Conditional
{
    typedef T value;
};


template<class T,class F>
struct Conditional<false,T,F>
{
    typedef F value;
};

template<bool Const,class T>
struct AddConstIf
{
    typedef typename Conditional<Const,const typename RemoveConst<T>::type,
                                        typename RemoveConst<T>::type>::value type;
};



template<class D,class V,class Tag,bool Const>
class IteratorFacade:public IteratorFacadeNoValueType<D,Tag,Const>
{
public:
    using value_type=typename AddConstIf<Const,V>::type;
//    using value_type=V;
    using reference=value_type&;
    using pointer=value_type*;
    using difference_type=std::ptrdiff_t;
    using iterator_category=Tag;

//    friend bool operator==(D const& lhs,D const& rhs) {
//        return equal(lhs,rhs);
//    }
//
//    friend bool operator!=(D const& lhs,D const& rhs) {
//        return !(lhs==rhs);
//    }

    //V&
    reference operator*() const {
        return derivedConst().dereference();
    }


    //V*
    //    pointer operator->() const {
    //        return std::addressof(*this->derivedConst());
    //    }
    pointer operator->() const {
        return std::addressof(operator*());
    }

//    D& operator++() {
//        derived().increment();
//        return derived();
//    }
//
//    D operator++(int) {
//        auto ret = derived(); // copy
//        derived().increment();
//        return ret;
//    }
//
//    D& operator--() {
//        derived().decrement();
//        return derived();
//    }
//
//    D operator--(int) {
//        // D tmp(this->derived());
//        auto ret = derived(); // copy
//        derived().decrement();
//        return ret;
//    }

//    pointer operator->() const {
//        return std::addressof(*this->derivedConst());
//    }

private:
    D &derived() {
        return static_cast<D&>(*this);
    }

    D const &derivedConst() const {
        return static_cast<D const&>(*this);
    }

    static bool equal(D const& lhs,D const& rhs) {
        return lhs.equal(rhs);
    }
};


template<class D,class I,class V,class Tag>
class IteratorAdaptor:public IteratorFacade<D,V,Tag, true>
{
public:
    using Super=IteratorFacade<D,V,Tag, true>;
    using value_type=typename Super::value_type;
    using iterator_category = typename Super::iterator_category;
    using reference = typename Super::reference;
    using pointer = typename Super::pointer;
    using difference_type = typename Super::difference_type;


    IteratorAdaptor()=default;
    explicit IteratorAdaptor(I base)
                            :base_(std::move(base))
    {}

    void increment() {
        ++base_;
    }

    void decrement() {
        --base_;
    }

    V& dereference() const {
        return *base_;
    }

    bool equal(D const& rhs) const {
        return base_ == rhs.base_;
    }

    I const& base() const {
        return base_;
    }

    I& base() {
        return base_;
    }
private:
    I base_;
};

}


/*
class IteratorCoreAccess
{
    template<class D,class V,class Tag>
    friend class IteratorFacade;

public:
    template<class Facade>
    static void increment(Facade& f) {
        f.increment();
    }
    template<class Facade>
    static void decrement(Facade &f) {
        f.decrement();
    }

    template<class Facade>
    static bool equal(Facade const &f1, Facade const &f2) {
        return f1.equal(f2);
    }

    template<class Facade>
    static typename Facade::reference dereference(Facade const &f) {
        return f.dereference();
    }

    // Objects of this class are useless.
    IteratorCoreAccess() = delete;
};

*/