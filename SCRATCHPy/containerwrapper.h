#ifndef CONTAINERWRAPPER_H
#define CONTAINERWRAPPER_H

#include <boost/python.hpp>
namespace p = boost::python;

#include <algorithm>

// For direct wrapping of c++ stl types
template<class T>
struct std_idx
{
    typedef typename T::value_type V;
    static V& get(T & x, int i)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) return x[i];
        IndexError();
    }
    static void set(T & x, int i, V const& v)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x[i]=v;
        else IndexError();
    }

    //operator== would need to be implemented for V
//    static bool in(T const& x, V const& v)
//    {
//            return std::find(x.begin(), x.end(), v) != x.end();
//    }

    static p::str repr(T const& x)
    {
        p::str res = "[";
        for (auto it = x.begin(); it != x.end(); ++it){
            if(it != x.begin())
                res += ", ";
            res += p::str(*it);
        }
        res +=  "]";
        return res;
    }

protected:
    static void IndexError() { PyErr_SetString(PyExc_IndexError, "Index out of range"); }
};

template<class T>
struct std_list : public std_idx<T>
{
    typedef typename T::value_type V;
    static void del(T & x, int i)
    {
        if( i<0 ) i+=x.size();
        if( i>=0 && i<x.size() ) x.erase(x.begin()+i);
        else std_idx<T>::IndexError();
    }
    static void add(T & x, V const& v)
    {
        x.push_back(v);
    }
};

template<typename T>
constexpr p::scope wrap_idx_container(char const* name) {
    return p::class_<T>(name)
        .def("__len__", &T::size)
        .def("__getitem__", &std_idx<T>::get,
              p::return_value_policy<p::reference_existing_object>())
        .def("__setitem__", &std_idx<T>::set,
              p::with_custodian_and_ward<1,2>()) // to let container keep value
        .def("__iter__", p::iterator<T>())
//        .def("__contains__", &std_idx<T>::in)
        .def("__repr__", &std_idx<T>::repr)
        ;
}

template<typename T>
constexpr p::scope wrap_list_container(char const* name) {
    return p::class_<T>(name)
        .def("__len__", &T::size)
        .def("clear", &T::clear)
        .def("append", &std_list<T>::add,
                p::with_custodian_and_ward<1,2>()) // to let container keep value
        .def("__getitem__", &std_list<T>::get,
              p::return_value_policy<p::reference_existing_object>())
        .def("__setitem__", &std_list<T>::set,
              p::with_custodian_and_ward<1,2>()) // to let container keep value
        .def("__delitem__", &std_list<T>::del)
        .def("__iter__", p::iterator<T>())
//        .def("__contains__", &std_list<T>::in)
        .def("__repr__", &std_list<T>::repr)
        ;
}

#endif // CONTAINERWRAPPER_H
