//===----------------- JavaArray.h - Java arrays --------------------------===//
//
//                            The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the internal representation of Java arrays.
//
//===----------------------------------------------------------------------===//

#ifndef JNJVM_JAVA_ARRAY_H
#define JNJVM_JAVA_ARRAY_H

#include "vmkit/Allocator.h"

#include "types.h"

#include "JavaObject.h"

#include "UTF8.h"

namespace j3 {

class ClassArray;
class CommonClass;
class JavaObject;
class Jnjvm;

/// TJavaArray - Template class to be instantiated by real arrays. All arrays
///  have a constant size and an array of element. When JnJVM allocates an
///  instantiation of this class, it allocates with the actual size of this
///  array. Hence instantiation of TJavaArrays have a layout of 
///  {JavaObject, size, [0 * T]}.
template <class T>
class TJavaArray : public JavaObject {
public:
  /// size - The (constant) size of the array.
  ssize_t size;

  /// elements - Elements of this array. The size here is different than the
  /// actual size of the Java array. This is to facilitate Java array accesses
  /// in JnJVM code. The size should be set to zero, but this is invalid C99.
  T elements[1];

  typedef T ElementType;

public:
  static int32_t getSize(const TJavaArray* self) __attribute__((always_inline)) {
    llvm_gcroot(self, 0);
    return self->size;
  }
  
  static T getElement(const TJavaArray* self, uint32_t i) __attribute__((always_inline)) {
    llvm_gcroot(self, 0);
    assert((ssize_t)i < self->size);
    return self->elements[i];
  }

  static void setElement(TJavaArray* self, T value, uint32_t i) __attribute__((always_inline)) {
    llvm_gcroot(self, 0);
    assert((ssize_t)i < self->size);
    self->elements[i] = value;
  }

  static const T* getElements(const TJavaArray* self) __attribute__((always_inline)) {
    llvm_gcroot(self, 0);
    return self->elements;
  }

  static T* getElements(TJavaArray* self) __attribute__((always_inline)) {
    llvm_gcroot(self, 0);
    return self->elements;
  }

  friend class JavaArray;
};

template<>
void TJavaArray<JavaObject*>::setElement(TJavaArray<JavaObject*>* self, JavaObject* value, uint32_t i);
typedef TJavaArray<JavaObject*> ArrayObject;

/// Instantiation of the TJavaArray class for Java arrays.
#define ARRAYCLASS(name, elmt)                                \
  class name : public TJavaArray<elmt> {                      \
  }

ARRAYCLASS(ArrayUInt8,  uint8);
ARRAYCLASS(ArraySInt8,  sint8);
ARRAYCLASS(ArrayUInt16, uint16);
ARRAYCLASS(ArraySInt16, sint16);
ARRAYCLASS(ArrayUInt32, uint32);
ARRAYCLASS(ArraySInt32, sint32);
ARRAYCLASS(ArrayLong,   sint64);
ARRAYCLASS(ArrayFloat,  float);
ARRAYCLASS(ArrayDouble, double);
ARRAYCLASS(ArrayPtr, word_t);

#undef ARRAYCLASS

/// JavaArray - This class is just a placeholder for constants.
class JavaArray {
public:
  /// MaxArraySize - The maximum size a Java array can have. Allocating an
  /// array with a bigger size than MaxArraySize raises an out of memory
  /// error.
  static const sint32 MaxArraySize;
  
  /// JVM representation of Java arrays of primitive types.
  static const unsigned int T_BOOLEAN;
  static const unsigned int T_CHAR;
  static const unsigned int T_FLOAT;
  static const unsigned int T_DOUBLE;
  static const unsigned int T_BYTE;
  static const unsigned int T_SHORT;
  static const unsigned int T_INT;
  static const unsigned int T_LONG;

  static void setSize(JavaObject* array, int size) __attribute__((always_inline)) {
    ArrayUInt8* obj = 0;
    llvm_gcroot(obj, 0);
    llvm_gcroot(array, 0);
    obj = (ArrayUInt8*)array;
    obj->size = size;
  }

  static sint32 getSize(const JavaObject* array) __attribute__((always_inline)) {
    const ArrayUInt8* obj = 0;
    llvm_gcroot(obj, 0);
    llvm_gcroot(array, 0);
    obj = (const ArrayUInt8*)array;
    return obj->size;
  }

  static const unsigned char* getElements(const JavaObject* array) __attribute__((always_inline)) {
    const ArrayUInt8* obj = 0;
    llvm_gcroot(obj, 0);
    llvm_gcroot(array, 0);
    obj = (const ArrayUInt8*)array;
    return obj->elements;
  }

  static unsigned char* getElements(JavaObject* array) __attribute__((always_inline)) {
    ArrayUInt8* obj = 0;
    llvm_gcroot(obj, 0);
    llvm_gcroot(array, 0);
    obj = (ArrayUInt8*)array;
    return obj->elements;
  }
};

} // end namespace j3

#endif
