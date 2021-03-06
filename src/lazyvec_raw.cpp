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
static R_altrep_class_t lazyvec_raw_class;


// [[Rcpp::export]]
SEXP lazyvec_raw_wrapper(SEXP data)
{
  return R_new_altrep(lazyvec_raw_class, data, NILSXP);
}


R_xlen_t lazyvec_raw_Length_method(SEXP x)
{
  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));

  // length listener method
  SEXP length_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_LENGTH));

  SEXP custom_length = PROTECT(call_r_interface(length_listener, user_data, LAZYVEC_PACKAGE_ENV(x)));
  
  // type and length checking is done on R side
  R_xlen_t res_length = (R_xlen_t)(*INTEGER(custom_length));
  
  UNPROTECT(3);  // custom_length, length_listener, user_data
  
  return res_length;
}


Rboolean lazyvec_raw_Inspect_method(SEXP x, int pre, int deep, int pvec,
  inspect_subtree_method subtree_method)
{
  return FALSE;
}


void* lazyvec_raw_Dataptr_method(SEXP x, Rboolean writeable)
{
  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));

  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));

  // length listener method
  SEXP full_vector_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_FULL_VECTOR));

  SEXP stored_full_vec = LAZYVEC_FULL_VEC(x);

  // return dataptr of stored vector
  if (!Rf_isNull(stored_full_vec)) {
    UNPROTECT(3);
    return DATAPTR(stored_full_vec);
  }

  // retrieve full vector
  SEXP full_vector = PROTECT(call_r_interface(full_vector_listener, user_data, calling_env));

  // add vector to user data
  LAZYVEC_SET_FULL_VEC(x, full_vector);

  UNPROTECT(4);

  return DATAPTR(full_vector);
}


const void *lazyvec_raw_Dataptr_or_null_method(SEXP x)
{
  SEXP stored_full_vec = LAZYVEC_FULL_VEC(x);
  
  // return dataptr of stored vector
  if (!Rf_isNull(stored_full_vec)) {
    return DATAPTR(stored_full_vec);
  }
  
  return NULL;
}


Rbyte lazyvec_raw_Elt_method(SEXP x, R_xlen_t i)
{
  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));
  
  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));
  
  // length listener method
  SEXP elt_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_ELT));

  // i argument
  SEXP i_arg = PROTECT(Rf_ScalarInteger((int)(i + 1)));

  // should return a length 1 vector containing the element
  SEXP custom_element = PROTECT(call_dual_r_interface(elt_listener, user_data, i_arg, calling_env));

  // convert to Rbyte
  Rbyte element = SEXP_TO_RBYTE(custom_element);

  UNPROTECT(5);
  
  return element;
}


R_xlen_t lazyvec_raw_Get_region_method(SEXP x, R_xlen_t i, R_xlen_t n, Rbyte *buf)
{
  // test for expanded vector here

  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));

  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));

  // length listener method
  SEXP get_region_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_GET_REGION));

  // i, n argument
  SEXP i_arg = PROTECT(Rf_ScalarInteger((int)(i + 1)));
  SEXP n_arg = PROTECT(Rf_ScalarInteger((int)(i)));

  // should return a length n vector containing the elements
  SEXP vec_elems = PROTECT(call_tripple_r_interface(get_region_listener, user_data, i_arg, n_arg, calling_env));

  // convert to C_TYPE
  Rbyte* elements = (Rbyte*)(RAW(vec_elems));

  memcpy(buf, elements, n * sizeof(Rbyte));

  UNPROTECT(6);

  return (R_xlen_t) LENGTH(vec_elems);
}


SEXP lazyvec_raw_DuplicateEX_method(SEXP x, Rboolean deep)
{
  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));
  
  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));
  
  // length listener method
  SEXP full_vector_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_FULL_VECTOR));
  
  SEXP stored_full_vec = LAZYVEC_FULL_VEC(x);
  
  // return dataptr of stored vector
  if (!Rf_isNull(stored_full_vec)) {
    UNPROTECT(3);
    return stored_full_vec;
  }

  // retrieve full vector
  SEXP full_vector = PROTECT(call_r_interface(full_vector_listener, user_data, calling_env));

  // add vector to user data
  LAZYVEC_SET_FULL_VEC(x, full_vector);

  UNPROTECT(4);

  return full_vector;
}


SEXP lazyvec_raw_Coerce_method(SEXP x, int type)
{
  SEXP type_descriptor;

  switch (type)
  {
    case 10:  // LGLSXP
      type_descriptor = PROTECT(Rf_mkString("logical"));
      break;
    case 13:  // INTSXP
      type_descriptor = PROTECT(Rf_mkString("integer"));
      break;
    case 14:  // REALSXP
      type_descriptor = PROTECT(Rf_mkString("double"));
      break;
    case 15:  // CPLXSXP
      type_descriptor = PROTECT(Rf_mkString("complex"));
      break;
    case 16: // STRSXP
      type_descriptor = PROTECT(Rf_mkString("character"));
      break;
    case 19: // VECSXP
      type_descriptor = PROTECT(Rf_mkString("list"));
      break;
    case 24:  // RAWSXP
      type_descriptor = PROTECT(Rf_mkString("raw"));
      break;
    case 20:  // EXPRSXP
      type_descriptor = PROTECT(Rf_mkString("expression"));
      break;
    default:
      Rf_error("Undefined type requested from R");
  }

  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));

  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));
  
  // coerce user method
  SEXP coerce_method = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_COERCE));

  // should return a coerced vector
  SEXP coerced_vec = call_dual_r_interface(
    coerce_method, user_data, type_descriptor, calling_env);

  UNPROTECT(4);  // type_descriptor, user_data, calling_env, coerce_method

  // use default conversion with vector expansion
  if (Rf_isNull(coerced_vec)) return NULL;

  return coerced_vec;
}


SEXP lazyvec_raw_Extract_subset_method(SEXP x, SEXP indx, SEXP call)
{
  // custom payload
  SEXP user_data = PROTECT(LAZYVEC_USER_DATA(x));
  
  // calling environment
  SEXP calling_env = PROTECT(LAZYVEC_PACKAGE_ENV(x));
  
  // extract_subset user method
  SEXP extract_subset_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_EXTRACT_SUBSET));

  // checks are for safety, remove later
  if (indx == NULL)
  {
    Rf_error("indx is_null");
  }

  if (call == NULL)
  {
    Rf_error("call is_null");
  }

  // should return a vector containing a subset of elements
  SEXP custom_elements = call_dual_r_interface(
    extract_subset_listener, user_data, indx, calling_env);

  UNPROTECT(3);  // extract_subset_listener, calling_env, user_data
  
  return custom_elements;
}


//
// On Win there is no Unserialize method exported, check with R-dev!
//
static SEXP lazyvec_raw_Unserialize_method(SEXP lazyvec_class, SEXP state)
{
  Rcpp::Environment pkgs = Rcpp::Environment::namespace_env("lazyvec");
  
  SEXP altrep_data1 = PROTECT(Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(altrep_data1, 0, VECTOR_ELT(state, 0));
  SET_VECTOR_ELT(altrep_data1, 1, VECTOR_ELT(state, 1));
  SET_VECTOR_ELT(altrep_data1, 2, VECTOR_ELT(state, 2));
  SET_VECTOR_ELT(altrep_data1, 3, pkgs);
  
  // unserialize listener method
  // SEXP unserialize_listener = PROTECT(VECTOR_ELT(VECTOR_ELT(state, 1), LAZYVEC_METHOD_UNSERIALIZE));
  
  // call_r_interface(unserialize_listener, state, LAZYVEC_PACKAGE_ENV(altrep_data1));
  
  // UNPROTECT(2);
  UNPROTECT(1);
  return lazyvec_raw_wrapper(altrep_data1);
}


//
// ALTREP_UNSERIALIZE_EX is not linking on linux due to uncommented hidden_attribute
// in declaration
//
SEXP lazyvec_raw_UnserializeEX_method(SEXP info, SEXP state, SEXP attr, int objf, int levs)
{
  // return ALTREP_UNSERIALIZE_EX_PROXY(info, state, attr, objf, levs);
  
  Rcpp::Environment pkgs = Rcpp::Environment::namespace_env("lazyvec");
  
  SEXP altrep_data1 = PROTECT(Rf_allocVector(VECSXP, 4));
  SET_VECTOR_ELT(altrep_data1, 0, VECTOR_ELT(state, 0));
  SET_VECTOR_ELT(altrep_data1, 1, VECTOR_ELT(state, 1));
  SET_VECTOR_ELT(altrep_data1, 2, VECTOR_ELT(state, 2));
  SET_VECTOR_ELT(altrep_data1, 3, pkgs);
  
  // SEXP unserialize_ex_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(altrep_data1),
  //   LAZYVEC_METHOD_UNSERIALIZE_EX));
  
  // Rf_PrintValue(state);
  // call_r_interface(unserialize_ex_listener, state, LAZYVEC_PACKAGE_ENV(altrep_data1));
  
  // UNPROTECT(2);
  UNPROTECT(1);
  return lazyvec_raw_wrapper(altrep_data1);
}


SEXP lazyvec_raw_Serialized_state_method(SEXP x)
{
  // SEXP serialized_state_result = PROTECT(ALTREP_SERIALIZED_STATE_PROXY(LAZYVEC_PAYLOAD(x)));
  
  // length listener method
  // SEXP serialized_state_listener = PROTECT(VECTOR_ELT(LAZYVEC_DIAGNOSTICS(x), LAZYVEC_METHOD_SERIALIZED_STATE));
  
  // create serialization state
  SEXP serialized_state = PROTECT(Rf_allocVector(VECSXP, 2));
  SET_VECTOR_ELT(serialized_state, 0, LAZYVEC_PAYLOAD(x));
  SET_VECTOR_ELT(serialized_state, 1, LAZYVEC_DIAGNOSTICS(x));
  
  // if (serialized_state_result == NULL)
  // {
  //   call_r_interface(serialized_state_listener, R_NilValue, LAZYVEC_PACKAGE_ENV(x));
  // }
  // else
  // {
  //   call_r_interface(serialized_state_listener, serialized_state_result, LAZYVEC_PACKAGE_ENV(x));
  // }
  
  UNPROTECT(3);
  
  return serialized_state;
}


// [[Rcpp::init]]
void register_lazyvec_raw_class(DllInfo *dll)
{
  lazyvec_raw_class = R_make_altraw_class("lazyvec_raw", "lazyvec", dll);

  /* override ALTREP methods */
  CALL_LAZYVEC_SETTER(altrep, raw, UnserializeEX);     // codeline: UnserializeEX
  CALL_LAZYVEC_SETTER(altrep, raw, Unserialize);       // codeline: Unserialize
  CALL_LAZYVEC_SETTER(altrep, raw, Serialized_state);  // codeline: Serialized_state
  CALL_LAZYVEC_SETTER(altrep, raw, DuplicateEX);       // codeline: DuplicateEx
  CALL_LAZYVEC_SETTER(altrep, raw, Coerce);            // codeline: Coerce
  CALL_LAZYVEC_SETTER(altrep, raw, Inspect);           // codeline: Inspect
  CALL_LAZYVEC_SETTER(altrep, raw, Length);            // codeline: Length

  /* override ALTVEC methods */
  CALL_LAZYVEC_SETTER(altvec, raw, Dataptr);           // codeline: Dataptr
  CALL_LAZYVEC_SETTER(altvec, raw, Dataptr_or_null);   // codeline: Dataptr_or_null
  CALL_LAZYVEC_SETTER(altvec, raw, Extract_subset);    // codeline: Extract_subset

  /* override specific type methods */
  CALL_LAZYVEC_SETTER(altraw, raw, Elt);           // codeline: Elt
  CALL_LAZYVEC_SETTER(altraw, raw, Get_region);    // codeline: Get_region
}
