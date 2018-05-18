
#include "api_helpers.h"
#include "Altrep.h"

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>

static R_altrep_class_t altvec_int_class;

SEXP construct_lazy_vec(SEXP data)
{
  SEXP state2 = Rf_mkString("data1");
  PROTECT(state2);

  SEXP altrep_vec = R_new_altrep(altvec_int_class, data, state2);

  UNPROTECT(1);  // state1, state2

  return altrep_vec;
}


static SEXP altvec_Unserialize(SEXP altvec_class, SEXP state)
{
  Rf_PrintValue(Rf_mkString("altvec_Unserialize called"));

  return Rf_ScalarLogical(1);
}


static SEXP altvec_Serialized_state(SEXP x)
{
  return NULL;
}


Rboolean altvec_Inspect(SEXP x, int pre, int deep, int pvec,
  void (*inspect_subtree)(SEXP, int, int, int))
{
  return TRUE;
}


#define ALTREP_INTERFACE(meta_data) VECTOR_ELT(meta_data, 0)
#define ALTREP_PARENT_ENV(meta_data) VECTOR_ELT(meta_data, 1)
#define ALTREP_INTERFACE_CLASS(meta_data) VECTOR_ELT(meta_data, 2);

static R_xlen_t altvec_Length(SEXP x)
{
  Rf_PrintValue(Rf_mkString("altvec_Length start"));

  SEXP meta_data = R_altrep_data1(x);

  SEXP interface_list         = ALTREP_INTERFACE(meta_data);
  SEXP parent_environment     = ALTREP_PARENT_ENV(meta_data);
  SEXP altrep_interface_class = ALTREP_INTERFACE_CLASS(meta_data);

  // length interface method
  SEXP method_length = VECTOR_ELT(interface_list, 0);

  // method return an integer value
  SEXP vec_lenth_r = call_r_interface(method_length, altrep_interface_class, parent_environment);

  R_xlen_t vec_length = (R_xlen_t)(*INTEGER(vec_lenth_r));

  Rf_PrintValue(vec_lenth_r);

  Rf_PrintValue(Rf_mkString("altvec_Length end"));

  return (R_xlen_t) (vec_length);
}


static void *altvec_Dataptr(SEXP x, Rboolean writeable)
{
  Rf_PrintValue(Rf_mkString("altvec_Dataptr called"));

  /* get addr first to get error if the object has been unmapped */
  return NULL;
}


static const void *altvec_Dataptr_or_null(SEXP x)
{
  Rf_PrintValue(Rf_mkString("altvec_Dataptr_or_null called"));

  return NULL;
}


static int altvec_integer_Elt(SEXP x, R_xlen_t i)
{
  Rf_PrintValue(Rf_mkString("altvec_integer_Elt called"));

  SEXP meta_data = R_altrep_data1(x);

  SEXP interface_list         = ALTREP_INTERFACE(meta_data);
  SEXP parent_environment     = ALTREP_PARENT_ENV(meta_data);
  SEXP altrep_interface_class = ALTREP_INTERFACE_CLASS(meta_data);

  // length interface method
  SEXP method_element = VECTOR_ELT(interface_list, 1);

  // method return an integer value
  SEXP vec_element_r = call_r_interface(method_element, altrep_interface_class, parent_environment);

  return *INTEGER(vec_element_r);
}


static R_xlen_t altvec_integer_Get_region(SEXP sx, R_xlen_t i, R_xlen_t n, int *buf)
{
  Rf_PrintValue(Rf_mkString("altvec_integer_Get_region called"));

  return 1;
}


void register_integer_class(DllInfo *dll)
{
  R_altrep_class_t int_class = R_make_altinteger_class("altvec_integer", "lazyvec", dll);
  altvec_int_class = int_class;

  /* override ALTREP methods */
  R_set_altrep_Unserialize_method(int_class, altvec_Unserialize);
  R_set_altrep_Serialized_state_method(int_class, altvec_Serialized_state);
  R_set_altrep_Inspect_method(int_class, altvec_Inspect);
  R_set_altrep_Length_method(int_class, altvec_Length);

  /* override ALTVEC methods */
  R_set_altvec_Dataptr_method(int_class, altvec_Dataptr);
  R_set_altvec_Dataptr_or_null_method(int_class, altvec_Dataptr_or_null);

  /* override ALTINTEGER methods */
  R_set_altinteger_Elt_method(int_class, altvec_integer_Elt);
  R_set_altinteger_Get_region_method(int_class, altvec_integer_Get_region);

  Rf_PrintValue(Rf_mkString("altvec_integer class registered"));
}
