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


// Get the exact ALTREP class descriptor
// [[Rcpp::export]]
void lazyvec_deexpand(SEXP lazy_vec) {
  LAZYVEC_SET_FULL_VEC(lazy_vec, R_NilValue);
}


// [[Rcpp::export]]
SEXP lazyvec_is_expanded(SEXP lazy_vec) {
  return Rf_ScalarLogical(LAZYVEC_FULL_VEC(lazy_vec) != R_NilValue);
}
