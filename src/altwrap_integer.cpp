//  lazyvec - R package for creating, testing and deploying custom ALTREP vectors
//
//  Copyright (C) 2019-present, Mark AJ Klik
//
//  This file is part of the lazyvec R package.
//
//  The lazyvec R package is free software: you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License version 3 as
//  published by the Free Software Foundation.
//
//  The lazyvec R package is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
//  for more details.
//
//  You should have received a copy of the GNU Affero General Public License along
//  with the lazyvec R package. If not, see <http://www.gnu.org/licenses/>.
//
//  You can contact the author at:
//  - lazyvec R package source repository : https://github.com/fstpackage/lazyvec


#include <Rcpp.h>

#include "api_helpers.h"

#include <stdint.h>


// altrep integer class definition
static R_altrep_class_t altwrap_integer_class;


// [[Rcpp::export]]
SEXP altrep_integer_wrapper(SEXP data)
{
  return R_new_altrep(altwrap_integer_class, data, NILSXP);
}


static SEXP altwrap_integer_Unserialize_method(SEXP altwrap_class, SEXP state)
{
  int serialized_version = INTEGER(VECTOR_ELT(state, SERIALIZED_VERSION))[0];  // lazyvec used for serialization
  
  if (serialized_version > LAZYVEC_VERSION)
  {
    Rf_error("Vector was serialized with a later lazyvec package version, please update lazyvec");
  }
  
  Rcpp::Environment pkgs = Rcpp::Environment::namespace_env("lazyvec");
  
  // picking up altrep_listener from the currently installed lazyvec
  Rcpp::Function altrep_listener = pkgs["altrep_listener"];
  
  // create a new wrapper using the current lazyvec implementation
  SEXP payload = PROTECT(VECTOR_ELT(state, SERIALIZED_PAYLOAD));
  SEXP wrapped_vec = PROTECT(altrep_listener(payload, VECTOR_ELT(state, SERIALIZED_METADATA)));
  
  // get listener from newly wrapped vector (no need to protect?)
  SEXP unserialize_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(wrapped_vec), ALTREP_METHOD_UNSERIALIZE));
  
  SEXP altrep_info = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(altrep_info, 0, sexp_or_null(ATTRIB(ALTREP_CLASS(payload))));
  SET_VECTOR_ELT(altrep_info, 1, sexp_or_null(VECTOR_ELT(state, SERIALIZED_STATE)));

  call_r_interface(unserialize_listener, altrep_info, pkgs);
  
  UNPROTECT(4);  // altrep_info, unserialize_listener, wrapped_vec, payload
  
  return wrapped_vec;
}


SEXP altwrap_integer_UnserializeEX_method(SEXP altwrap_class, SEXP state, SEXP attr, int objf, int levs)
{
  int serialized_version = INTEGER(VECTOR_ELT(state, SERIALIZED_VERSION))[0];  // lazyvec used for serialization
  
  if (serialized_version > LAZYVEC_VERSION)
  {
    Rf_error("Vector was serialized with a later lazyvec package version, please update lazyvec");
  }
  
  Rcpp::Environment pkgs = Rcpp::Environment::namespace_env("lazyvec");
  
  // picking up altrep_listener from the currently installed lazyvec
  Rcpp::Function altrep_listener = pkgs["altrep_listener"];
  
  // create a new wrapper using the current lazyvec implementation
  SEXP payload = PROTECT(VECTOR_ELT(state, SERIALIZED_PAYLOAD));
  SEXP wrapped_vec = PROTECT(altrep_listener(payload, VECTOR_ELT(state, SERIALIZED_METADATA)));
  
  // get listener from newly wrapped vector (no need to protect?)
  SEXP unserialize_ex_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(wrapped_vec), ALTREP_METHOD_UNSERIALIZE_EX));
  
  SEXP altrep_info = PROTECT(Rf_allocVector(VECSXP, 5));
  SET_VECTOR_ELT(altrep_info, 0, sexp_or_null(ATTRIB(ALTREP_CLASS(payload))));
  SET_VECTOR_ELT(altrep_info, 1, sexp_or_null(VECTOR_ELT(state, SERIALIZED_STATE)));
  SET_VECTOR_ELT(altrep_info, 2, sexp_or_null(attr));
  SET_VECTOR_ELT(altrep_info, 3, Rf_ScalarInteger(objf));
  SET_VECTOR_ELT(altrep_info, 4, Rf_ScalarInteger(levs));
  
  call_r_interface(unserialize_ex_listener, altrep_info, pkgs);
  
  UNPROTECT(4);  // altrep_info, unserialize_ex_listener, wrapped_vec, payload
  
  return wrapped_vec;
}


SEXP altwrap_integer_Serialized_state_method(SEXP x)
{
  SEXP serialized_state_result = PROTECT(ALTREP_SERIALIZED_STATE_PROXY(ALTWRAP_PAYLOAD(x)));
  
  // length listener method
  SEXP serialized_state_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_SERIALIZED_STATE));
  
  // create serialization state
  SEXP serialized_state = PROTECT(Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(serialized_state, SERIALIZED_PAYLOAD,  ALTWRAP_PAYLOAD(x));
  SET_VECTOR_ELT(serialized_state, SERIALIZED_METADATA, ALTWRAP_METADATA(x));
  SET_VECTOR_ELT(serialized_state, SERIALIZED_VERSION,  ALTWRAP_VERSION(x));

  SEXP res = PROTECT(sexp_or_null(serialized_state_result));
  SET_VECTOR_ELT(serialized_state, SERIALIZED_STATE, res);
  call_r_interface(serialized_state_listener, res, ALTWRAP_PARENT_ENV(x));
  
  UNPROTECT(4);
  
  return serialized_state;
}


Rboolean altwrap_integer_Inspect_method(SEXP x, int pre, int deep, int pvec,
  inspect_subtree_method subtree_method)
{
  Rboolean inspect_result = ALTREP_INSPECT_PROXY(ALTWRAP_PAYLOAD(x), pre, deep, pvec, subtree_method);

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 4));

  SET_VECTOR_ELT(arguments, 0, Rf_ScalarInteger(inspect_result));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(pre));
  SET_VECTOR_ELT(arguments, 2, Rf_ScalarInteger(deep));
  SET_VECTOR_ELT(arguments, 3, Rf_ScalarInteger(pvec));

  // length listener method
  SEXP inspect_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_INSPECT));

  // call inspect listener
  call_r_interface(inspect_listener, arguments, ALTWRAP_PARENT_ENV(x));

  UNPROTECT(2);

  return inspect_result;
}


R_xlen_t altwrap_integer_Length_method(SEXP x)
{
  R_xlen_t length_result = ALTREP_LENGTH(ALTWRAP_PAYLOAD(x));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(x));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(length_result));

  // length listener method
  SEXP length_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_LENGTH));

  // call listener with integer length result
  // TODO: change to int64 result
  call_r_interface(length_listener, arguments, ALTWRAP_PARENT_ENV(x));

  UNPROTECT(2);

  return length_result;
}


void* altwrap_integer_Dataptr_method(SEXP x, Rboolean writeable)
{
  // create structure with info
  SEXP arguments = PROTECT(Rf_allocVector(INTSXP, 3));

  int* parguments = INTEGER(arguments);
  void* pdata = ALTVEC_DATAPTR(ALTWRAP_PAYLOAD(x));

  intptr_t address = (intptr_t) pdata;

  parguments[0] = (int) ((address >> 32) & 0xffffffff);
  parguments[1] = (int) (address & 0xffffffff);
  parguments[2] = (int) writeable;

  // call listener with info

  // dataptr_or_null listener method
  SEXP dataptr_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_DATAPTR));

  // call listener with integer result
  call_r_interface(dataptr_listener, arguments, ALTWRAP_PARENT_ENV(x));

  UNPROTECT(2);  // arguments

  return pdata;
}


const void *altwrap_integer_Dataptr_or_null_method(SEXP x)
{
  const void* pdata_or_null = DATAPTR_OR_NULL(ALTWRAP_PAYLOAD(x));
  int is_pointer = pdata_or_null == NULL;

  // dataptr_or_null listener method
  SEXP dataptr_or_null_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_DATAPTR_OR_NULL));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(x));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarLogical(is_pointer));
  
  // call listener with integer result
  call_r_interface(dataptr_or_null_listener, arguments, ALTWRAP_PARENT_ENV(x));

  UNPROTECT(2);

  return pdata_or_null;
}


int altwrap_integer_Elt_method(SEXP sx, R_xlen_t i)
{
  int element = INTEGER_ELT(ALTWRAP_PAYLOAD(sx), i);

  // retrieve is_sorted listener method
  SEXP elt_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_ELT));

  // arguments vector
  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(sx));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(element));

  // call listener with result with the correct ALTREP type
  call_r_interface(elt_listener, arguments, ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(2);

  return element;
}


R_xlen_t altwrap_integer_Get_region_method(SEXP sx, R_xlen_t i, R_xlen_t n, int *buf)
{
  R_xlen_t length = INTEGER_GET_REGION(ALTWRAP_PAYLOAD(sx), i, n, buf);

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(sx));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(i));
  SET_VECTOR_ELT(arguments, 2, Rf_ScalarInteger(n));
  SET_VECTOR_ELT(arguments, 3, Rf_ScalarInteger(length));

  // dataptr_or_null listener method
  SEXP get_region_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_GET_REGION));

  // call listener with integer result
  call_r_interface(get_region_listener, arguments, ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(2);

  return length;
}


int altwrap_integer_Is_sorted_method(SEXP sx)
{
  int is_sorted = INTEGER_IS_SORTED(ALTWRAP_PAYLOAD(sx));

  // retrieve is_sorted listener method
  SEXP is_sorted_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_IS_SORTED));

  // call listener with integer result
  call_r_interface(is_sorted_listener, Rf_ScalarInteger(is_sorted), ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(1);

  return is_sorted;
}


int altwrap_integer_No_NA_method(SEXP sx)
{
  int no_na = INTEGER_NO_NA(ALTWRAP_PAYLOAD(sx));

  // retrieve no_na listener method
  SEXP no_na_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_NO_NA));

  // call listener with integer result
  call_r_interface(no_na_listener, Rf_ScalarInteger(no_na), ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(1);

  return no_na;
}


SEXP altwrap_integer_Sum_method(SEXP sx, Rboolean na_rm)
{
  SEXP sum = PROTECT(ALTINTEGER_SUM(ALTWRAP_PAYLOAD(sx), na_rm));

  // retrieve sum listener method
  SEXP sum_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_SUM));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(na_rm));

  if (sum == NULL)
  {
    SET_VECTOR_ELT(arguments, 0, R_NilValue);

    // call listener with integer result
    call_r_interface(sum_listener, arguments, ALTWRAP_PARENT_ENV(sx));

    UNPROTECT(3);

    return sum;
  }

  SET_VECTOR_ELT(arguments, 0, sum);

  // call listener with integer result
  call_r_interface(sum_listener, arguments, ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(3);

  return sum;
}


SEXP altwrap_integer_Min_method(SEXP sx, Rboolean na_rm)
{
  SEXP result_min = PROTECT(ALTINTEGER_MIN(ALTWRAP_PAYLOAD(sx), na_rm));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(sx));
  SET_VECTOR_ELT(arguments, 1, result_min == NULL ? R_NilValue : result_min);
  
  // retrieve min listener method
  SEXP min_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_MIN));

  // call listener with SEXP result
  call_r_interface(min_listener, arguments, ALTWRAP_PARENT_ENV(sx));
  UNPROTECT(3);

  return result_min;
}


SEXP altwrap_integer_Max_method(SEXP sx, Rboolean na_rm)
{
  SEXP result_max = PROTECT(ALTINTEGER_MAX(ALTWRAP_PAYLOAD(sx), na_rm));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(sx));
  SET_VECTOR_ELT(arguments, 1, result_max == NULL ? R_NilValue : result_max);
  
  // retrieve max listener method
  SEXP max_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_MAX));

  // call listener with SEXP result
  call_r_interface(max_listener, arguments, ALTWRAP_PARENT_ENV(sx));
  UNPROTECT(3);
  
  return result_max;
}


SEXP altwrap_integer_DuplicateEX_method(SEXP x, Rboolean deep)
{
  SEXP result_duplicate_ex = PROTECT(ALTREP_DUPLICATE_EX_PROXY(ALTWRAP_PAYLOAD(x), deep));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 3));
  SET_VECTOR_ELT(arguments, 0, ALTWRAP_METADATA(x));
  SET_VECTOR_ELT(arguments, 1, result_duplicate_ex == NULL ? R_NilValue : result_duplicate_ex);
  SET_VECTOR_ELT(arguments, 2, Rf_ScalarLogical(deep));

  // retrieve duplicateEX listener method
  SEXP duplicate_ex_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(x), ALTREP_METHOD_DUPLICATE_EX));

  call_r_interface(duplicate_ex_listener, arguments, ALTWRAP_PARENT_ENV(x));
  UNPROTECT(3);

  return result_duplicate_ex;
}


SEXP altwrap_integer_Coerce_method(SEXP sx, int type)
{
  SEXP result_coerce = PROTECT(ALTREP_COERCE_PROXY(ALTWRAP_PAYLOAD(sx), type));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(arguments, 1, Rf_ScalarInteger(type));

  // retrieve coerce listener method
  SEXP coerce_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_COERCE));

  if (result_coerce == NULL)
  {
    SET_VECTOR_ELT(arguments, 0, R_NilValue);

    // call listener with integer result
    call_r_interface(coerce_listener, arguments, ALTWRAP_PARENT_ENV(sx));

    UNPROTECT(3);
    return result_coerce;
  }

  SET_VECTOR_ELT(arguments, 0, result_coerce);

  // call listener with integer result
  call_r_interface(coerce_listener, arguments, ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(3);
  return result_coerce;
}


SEXP altwrap_integer_Extract_subset_method(SEXP sx, SEXP indx, SEXP call)
{
  SEXP result_extract_subset = PROTECT(ALTVEC_EXTRACT_SUBSET_PROXY(ALTWRAP_PAYLOAD(sx), indx, call));

  SEXP arguments = PROTECT(Rf_allocVector(VECSXP, 3));

  if (result_extract_subset == NULL)
  {
    SET_VECTOR_ELT(arguments, 0, R_NilValue);
  } else
  {
    SET_VECTOR_ELT(arguments, 0, result_extract_subset);
  }

  if (indx == NULL)
  {
    SET_VECTOR_ELT(arguments, 1, R_NilValue);
  } else
  {
    SET_VECTOR_ELT(arguments, 1, indx);
  }

  if (call == NULL)
  {
    SET_VECTOR_ELT(arguments, 2, R_NilValue);
  } else
  {
    SET_VECTOR_ELT(arguments, 2, call);
  }

  // retrieve coerce listener method
  SEXP extract_subset_listener = PROTECT(VECTOR_ELT(ALTWRAP_LISTENERS(sx), ALTREP_METHOD_EXTRACT_SUBSET));

  // call listener with arguments and result
  call_r_interface(extract_subset_listener, arguments, ALTWRAP_PARENT_ENV(sx));

  UNPROTECT(3);

  return result_extract_subset;
}


// [[Rcpp::init]]
void register_altrep_integer_class(DllInfo *dll)
{
  altwrap_integer_class = R_make_altinteger_class("altwrap_integer", "lazyvec", dll);

  /* override ALTREP methods */
  CALL_METHOD_SETTER(altrep, integer, UnserializeEX);     // codeline: UnserializeEX
  CALL_METHOD_SETTER(altrep, integer, Unserialize);       // codeline: Unserialize
  CALL_METHOD_SETTER(altrep, integer, Serialized_state);  // codeline: Serialized_state
  CALL_METHOD_SETTER(altrep, integer, DuplicateEX);       // codeline: DuplicateEx
  CALL_METHOD_SETTER(altrep, integer, Coerce);            // codeline: Coerce
  CALL_METHOD_SETTER(altrep, integer, Inspect);           // codeline: Inspect
  CALL_METHOD_SETTER(altrep, integer, Length);            // codeline: Length

  /* override ALTVEC methods */
  CALL_METHOD_SETTER(altvec, integer, Dataptr);           // codeline: Dataptr
  CALL_METHOD_SETTER(altvec, integer, Dataptr_or_null);   // codeline: Dataptr_or_null
  CALL_METHOD_SETTER(altvec, integer, Extract_subset);    // codeline: Extract_subset

  /* override specific type methods */
  CALL_METHOD_SETTER(altinteger, integer, Elt);           // codeline: Elt
  CALL_METHOD_SETTER(altinteger, integer, Get_region);    // codeline: Get_region
  CALL_METHOD_SETTER(altinteger, integer, Is_sorted);     // codeline: Is_sorted
  CALL_METHOD_SETTER(altinteger, integer, No_NA);         // codeline: No_NA
  CALL_METHOD_SETTER(altinteger, integer, Sum);           // codeline: Sum
  CALL_METHOD_SETTER(altinteger, integer, Min);           // codeline: Min
  CALL_METHOD_SETTER(altinteger, integer, Max);           // codeline: Max
}
