// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME srcdILParticle_dict

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

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "inc/LParticle.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void *new_LParticle(void *p = 0);
   static void *newArray_LParticle(Long_t size, void *p);
   static void delete_LParticle(void *p);
   static void deleteArray_LParticle(void *p);
   static void destruct_LParticle(void *p);
   static void streamer_LParticle(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::LParticle*)
   {
      ::LParticle *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::LParticle >(0);
      static ::ROOT::TGenericClassInfo 
         instance("LParticle", ::LParticle::Class_Version(), "inc/LParticle.h", 14,
                  typeid(::LParticle), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::LParticle::Dictionary, isa_proxy, 16,
                  sizeof(::LParticle) );
      instance.SetNew(&new_LParticle);
      instance.SetNewArray(&newArray_LParticle);
      instance.SetDelete(&delete_LParticle);
      instance.SetDeleteArray(&deleteArray_LParticle);
      instance.SetDestructor(&destruct_LParticle);
      instance.SetStreamerFunc(&streamer_LParticle);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::LParticle*)
   {
      return GenerateInitInstanceLocal((::LParticle*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::LParticle*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr LParticle::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *LParticle::Class_Name()
{
   return "LParticle";
}

//______________________________________________________________________________
const char *LParticle::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::LParticle*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int LParticle::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::LParticle*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *LParticle::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::LParticle*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *LParticle::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::LParticle*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void LParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class LParticle.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> m_type;
      R__b >> m_status;
      R__b >> m_charge;
      R__b >> m_parent;
      momentum.Streamer(R__b);
      {
         vector<double> &R__stl =  parameters;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            double R__t;
            R__b >> R__t;
            R__stl.push_back(R__t);
         }
      }
      {
         vector<CParticle> &R__stl =  constituents;
         R__stl.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            CParticle R__t;
            R__t.Streamer(R__b);
            R__stl.push_back(R__t);
         }
      }
      R__b.CheckByteCount(R__s, R__c, LParticle::IsA());
   } else {
      R__c = R__b.WriteVersion(LParticle::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << m_type;
      R__b << m_status;
      R__b << m_charge;
      R__b << m_parent;
      momentum.Streamer(R__b);
      {
         vector<double> &R__stl =  parameters;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<double>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            R__b << (*R__k);
            }
         }
      }
      {
         vector<CParticle> &R__stl =  constituents;
         int R__n=int(R__stl.size());
         R__b << R__n;
         if(R__n) {
            vector<CParticle>::iterator R__k;
            for (R__k = R__stl.begin(); R__k != R__stl.end(); ++R__k) {
            ((CParticle&)(*R__k)).Streamer(R__b);
            }
         }
      }
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_LParticle(void *p) {
      return  p ? new(p) ::LParticle : new ::LParticle;
   }
   static void *newArray_LParticle(Long_t nElements, void *p) {
      return p ? new(p) ::LParticle[nElements] : new ::LParticle[nElements];
   }
   // Wrapper around operator delete
   static void delete_LParticle(void *p) {
      delete ((::LParticle*)p);
   }
   static void deleteArray_LParticle(void *p) {
      delete [] ((::LParticle*)p);
   }
   static void destruct_LParticle(void *p) {
      typedef ::LParticle current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_LParticle(TBuffer &buf, void *obj) {
      ((::LParticle*)obj)->::LParticle::Streamer(buf);
   }
} // end of namespace ROOT for class ::LParticle

namespace ROOT {
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = 0);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 210,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<double>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)0x0)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *vectorlECParticlegR_Dictionary();
   static void vectorlECParticlegR_TClassManip(TClass*);
   static void *new_vectorlECParticlegR(void *p = 0);
   static void *newArray_vectorlECParticlegR(Long_t size, void *p);
   static void delete_vectorlECParticlegR(void *p);
   static void deleteArray_vectorlECParticlegR(void *p);
   static void destruct_vectorlECParticlegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<CParticle>*)
   {
      vector<CParticle> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<CParticle>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<CParticle>", -2, "vector", 210,
                  typeid(vector<CParticle>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlECParticlegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<CParticle>) );
      instance.SetNew(&new_vectorlECParticlegR);
      instance.SetNewArray(&newArray_vectorlECParticlegR);
      instance.SetDelete(&delete_vectorlECParticlegR);
      instance.SetDeleteArray(&deleteArray_vectorlECParticlegR);
      instance.SetDestructor(&destruct_vectorlECParticlegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<CParticle> >()));
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<CParticle>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlECParticlegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<CParticle>*)0x0)->GetClass();
      vectorlECParticlegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlECParticlegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlECParticlegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<CParticle> : new vector<CParticle>;
   }
   static void *newArray_vectorlECParticlegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<CParticle>[nElements] : new vector<CParticle>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlECParticlegR(void *p) {
      delete ((vector<CParticle>*)p);
   }
   static void deleteArray_vectorlECParticlegR(void *p) {
      delete [] ((vector<CParticle>*)p);
   }
   static void destruct_vectorlECParticlegR(void *p) {
      typedef vector<CParticle> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<CParticle>

namespace {
  void TriggerDictionaryInitialization_LParticle_dict_Impl() {
    static const char* headers[] = {
"inc/LParticle.h",
0
    };
    static const char* includePaths[] = {
"/users/admin/share/sl7/root_v6.12.06_gcc485/include",
"/users/chekanov/work/FastSim4ATLAS/delphes/FastSimNN/makedata/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "LParticle_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(Monte Carlo particle object)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$inc/LParticle.h")))  LParticle;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "LParticle_dict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "inc/LParticle.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"LParticle", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("LParticle_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_LParticle_dict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_LParticle_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_LParticle_dict() {
  TriggerDictionaryInitialization_LParticle_dict_Impl();
}
