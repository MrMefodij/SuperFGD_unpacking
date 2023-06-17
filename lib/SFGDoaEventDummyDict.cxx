// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME SFGDoaEventDummyDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "ToaEventDummy.h"
#include "TDigitDummy.h"
#include "TSFGDigit.hxx"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *TDigitDummy_Dictionary();
   static void TDigitDummy_TClassManip(TClass*);
   static void *new_TDigitDummy(void *p = nullptr);
   static void *newArray_TDigitDummy(Long_t size, void *p);
   static void delete_TDigitDummy(void *p);
   static void deleteArray_TDigitDummy(void *p);
   static void destruct_TDigitDummy(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TDigitDummy*)
   {
      ::TDigitDummy *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TDigitDummy));
      static ::ROOT::TGenericClassInfo 
         instance("TDigitDummy", "TDigitDummy.h", 9,
                  typeid(::TDigitDummy), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TDigitDummy_Dictionary, isa_proxy, 0,
                  sizeof(::TDigitDummy) );
      instance.SetNew(&new_TDigitDummy);
      instance.SetNewArray(&newArray_TDigitDummy);
      instance.SetDelete(&delete_TDigitDummy);
      instance.SetDeleteArray(&deleteArray_TDigitDummy);
      instance.SetDestructor(&destruct_TDigitDummy);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TDigitDummy*)
   {
      return GenerateInitInstanceLocal((::TDigitDummy*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TDigitDummy*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TDigitDummy_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TDigitDummy*)nullptr)->GetClass();
      TDigitDummy_TClassManip(theClass);
   return theClass;
   }

   static void TDigitDummy_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *ToaEventDummy_Dictionary();
   static void ToaEventDummy_TClassManip(TClass*);
   static void *new_ToaEventDummy(void *p = nullptr);
   static void *newArray_ToaEventDummy(Long_t size, void *p);
   static void delete_ToaEventDummy(void *p);
   static void deleteArray_ToaEventDummy(void *p);
   static void destruct_ToaEventDummy(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::ToaEventDummy*)
   {
      ::ToaEventDummy *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::ToaEventDummy));
      static ::ROOT::TGenericClassInfo 
         instance("ToaEventDummy", "ToaEventDummy.h", 10,
                  typeid(::ToaEventDummy), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &ToaEventDummy_Dictionary, isa_proxy, 0,
                  sizeof(::ToaEventDummy) );
      instance.SetNew(&new_ToaEventDummy);
      instance.SetNewArray(&newArray_ToaEventDummy);
      instance.SetDelete(&delete_ToaEventDummy);
      instance.SetDeleteArray(&deleteArray_ToaEventDummy);
      instance.SetDestructor(&destruct_ToaEventDummy);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::ToaEventDummy*)
   {
      return GenerateInitInstanceLocal((::ToaEventDummy*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::ToaEventDummy*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *ToaEventDummy_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::ToaEventDummy*)nullptr)->GetClass();
      ToaEventDummy_TClassManip(theClass);
   return theClass;
   }

   static void ToaEventDummy_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_TDigitDummy(void *p) {
      return  p ? new(p) ::TDigitDummy : new ::TDigitDummy;
   }
   static void *newArray_TDigitDummy(Long_t nElements, void *p) {
      return p ? new(p) ::TDigitDummy[nElements] : new ::TDigitDummy[nElements];
   }
   // Wrapper around operator delete
   static void delete_TDigitDummy(void *p) {
      delete ((::TDigitDummy*)p);
   }
   static void deleteArray_TDigitDummy(void *p) {
      delete [] ((::TDigitDummy*)p);
   }
   static void destruct_TDigitDummy(void *p) {
      typedef ::TDigitDummy current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TDigitDummy

namespace ROOT {
   // Wrappers around operator new
   static void *new_ToaEventDummy(void *p) {
      return  p ? new(p) ::ToaEventDummy : new ::ToaEventDummy;
   }
   static void *newArray_ToaEventDummy(Long_t nElements, void *p) {
      return p ? new(p) ::ToaEventDummy[nElements] : new ::ToaEventDummy[nElements];
   }
   // Wrapper around operator delete
   static void delete_ToaEventDummy(void *p) {
      delete ((::ToaEventDummy*)p);
   }
   static void deleteArray_ToaEventDummy(void *p) {
      delete [] ((::ToaEventDummy*)p);
   }
   static void destruct_ToaEventDummy(void *p) {
      typedef ::ToaEventDummy current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::ToaEventDummy

namespace ROOT {
   static TClass *vectorlEToaEventDummygR_Dictionary();
   static void vectorlEToaEventDummygR_TClassManip(TClass*);
   static void *new_vectorlEToaEventDummygR(void *p = nullptr);
   static void *newArray_vectorlEToaEventDummygR(Long_t size, void *p);
   static void delete_vectorlEToaEventDummygR(void *p);
   static void deleteArray_vectorlEToaEventDummygR(void *p);
   static void destruct_vectorlEToaEventDummygR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<ToaEventDummy>*)
   {
      vector<ToaEventDummy> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<ToaEventDummy>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<ToaEventDummy>", -2, "c++/v1/vector", 493,
                  typeid(vector<ToaEventDummy>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEToaEventDummygR_Dictionary, isa_proxy, 4,
                  sizeof(vector<ToaEventDummy>) );
      instance.SetNew(&new_vectorlEToaEventDummygR);
      instance.SetNewArray(&newArray_vectorlEToaEventDummygR);
      instance.SetDelete(&delete_vectorlEToaEventDummygR);
      instance.SetDeleteArray(&deleteArray_vectorlEToaEventDummygR);
      instance.SetDestructor(&destruct_vectorlEToaEventDummygR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<ToaEventDummy> >()));

      ::ROOT::AddClassAlternate("vector<ToaEventDummy>","std::__1::vector<ToaEventDummy, std::__1::allocator<ToaEventDummy>>");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<ToaEventDummy>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEToaEventDummygR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<ToaEventDummy>*)nullptr)->GetClass();
      vectorlEToaEventDummygR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEToaEventDummygR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEToaEventDummygR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ToaEventDummy> : new vector<ToaEventDummy>;
   }
   static void *newArray_vectorlEToaEventDummygR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<ToaEventDummy>[nElements] : new vector<ToaEventDummy>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEToaEventDummygR(void *p) {
      delete ((vector<ToaEventDummy>*)p);
   }
   static void deleteArray_vectorlEToaEventDummygR(void *p) {
      delete [] ((vector<ToaEventDummy>*)p);
   }
   static void destruct_vectorlEToaEventDummygR(void *p) {
      typedef vector<ToaEventDummy> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<ToaEventDummy>

namespace {
  void TriggerDictionaryInitialization_libSFGDoaEventDummyDict_Impl() {
    static const char* headers[] = {
"ToaEventDummy.h",
"TDigitDummy.h",
"TSFGDigit.hxx",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/SuperFGD_unpacking/.",
"/Users/maria/SuperFGD_unpacking/src",
"/Users/maria/SuperFGD_unpacking/oaEventDummy",
"/Applications/root_v6.26.06/include",
"/Users/maria/SuperFGD_unpacking/oaEventDummy/.",
"/Applications/root_v6.26.06/include/",
"/Users/maria/SuperFGD_unpacking/build/oaEventDummy/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libSFGDoaEventDummyDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$ToaEventDummy.h")))  ToaEventDummy;
namespace std{inline namespace __1{template <class _Tp> class __attribute__((annotate("$clingAutoload$iosfwd")))  __attribute__((annotate("$clingAutoload$string")))  allocator;
}}
class __attribute__((annotate("$clingAutoload$TDigitDummy.h")))  __attribute__((annotate("$clingAutoload$ToaEventDummy.h")))  TDigitDummy;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libSFGDoaEventDummyDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "ToaEventDummy.h"
#include "TDigitDummy.h"
#include "TSFGDigit.hxx"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TDigitDummy", payloadCode, "@",
"ToaEventDummy", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libSFGDoaEventDummyDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libSFGDoaEventDummyDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libSFGDoaEventDummyDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libSFGDoaEventDummyDict() {
  TriggerDictionaryInitialization_libSFGDoaEventDummyDict_Impl();
}
