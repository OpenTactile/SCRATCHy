#include "scratchpy.h"

#include <vector>
#include <QString>

template <class T>
struct Vector_to_PythonList {
    static PyObject* convert(std::vector<T> const& vector) {
        p::list list;
        for(auto iter : vector) {
            list.append(iter);
        }
        return p::incref(list.ptr());
    }
};

struct QString_to_PythonStr
{
    static PyObject* convert(QString const& s) {
        return p::incref(p::object(s.toLatin1().constData()).ptr());
    }
};
 
struct QString_from_PythonStr
{
    QString_from_PythonStr()
    {
      boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        boost::python::type_id<QString>());
    }
 
    // Determine if obj_ptr can be converted in a QString
    static void* convertible(PyObject* obj_ptr)
    {
      if (!PyUnicode_Check(obj_ptr)) return 0;
      return obj_ptr;
    }
 
    // Convert obj_ptr into a QString
    static void construct(
    PyObject* obj_ptr,
    boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      // Extract the character data from the python string
      const char* value = PyUnicode_AsUTF8(obj_ptr);
 
      // Verify that obj_ptr is a string (should be ensured by convertible())
      assert(value);
 
      // Grab pointer to memory into which to construct the new QString
      void* storage = (
        (boost::python::converter::rvalue_from_python_storage<QString>*)
        data)->storage.bytes;
 
      // in-place construct the new QString using the character data
      // extraced from the python object
      new (storage) QString(value);
 
      // Stash the memory chunk pointer for later use by boost.python
      data->convertible = storage;
    }
};

//struct FrequencyTable_from_python_list
//{
//    QString_from_python_str()
//    {
//      boost::python::converter::registry::push_back(
//        &convertible,
//        &construct,
//        boost::python::type_id<QString>());
//    }

//    // Determine if obj_ptr can be converted in a QString
//    static void* convertible(PyObject* obj_ptr)
//    {
//      if (!PyList_Check(obj_ptr)) return 0;
//      //if (!PyList_Si)
//      return obj_ptr;
//    }

//    // Convert obj_ptr into a QString
//    static void construct(
//    PyObject* obj_ptr,
//    boost::python::converter::rvalue_from_python_stage1_data* data)
//    {
//      // Extract the character data from the python string
//      const char* value = PyUnicode_AsUTF8(obj_ptr);

//      // Verify that obj_ptr is a string (should be ensured by convertible())
//      assert(value);

//      // Grab pointer to memory into which to construct the new QString
//      void* storage = (
//        (boost::python::converter::rvalue_from_python_storage<QString>*)
//        data)->storage.bytes;

//      // in-place construct the new QString using the character data
//      // extraced from the python object
//      new (storage) QString(value);

//      // Stash the memory chunk pointer for later use by boost.python
//      data->convertible = storage;
//    }
//};

void initialize_converters()
{
  // register the to-python converter
  p::to_python_converter<
    QString,
    QString_to_PythonStr>();

  p::to_python_converter<
    std::vector<SignalGenerator>,
    Vector_to_PythonList<SignalGenerator> >();

  p::to_python_converter<
    std::vector<uint8_t>,
    Vector_to_PythonList<uint8_t> >();

  // register the from-python converter
  QString_from_PythonStr();
}
