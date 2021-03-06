#  lazyvec - R package for creating, testing and deploying custom ALTREP vectors
#
#  Copyright (C) 2019-present, Mark AJ Klik
#
#  This file is part of the lazyvec R package.
#
#  The lazyvec R package is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Affero General Public License version 3 as
#  published by the Free Software Foundation.
#
#  The lazyvec R package is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License
#  for more details.
#
#  You should have received a copy of the GNU Affero General Public License along
#  with the lazyvec R package. If not, see <http://www.gnu.org/licenses/>.
#
#  You can contact the author at:
#  - lazyvec R package source repository : https://github.com/fstpackage/lazyvec


#' Define a custom ALTREP vector
#' 
#' This method creates a user-defined ALTREP vector with the complete ALTREP API created
#' in R instead of C/C++ sources.
#'
#' @param metadata custom metadata stored alongside the generated ALTREP vector.
#' @param vec_type data type required for the ALTREP vector, options are 'integer', 'double',
#' 'logical', 'raw' and 'character'. 
#' @param altrep_methods user-defined methods used by the resulting ALTREP vector.
#' @param package_environment package environment in which to evaluate the user methods. Defaults
#' to the lazyvec package.
#' @param diagnostics if TRUE, heavy type and boundary checks are performed before returning
#' the results of user defined methods to R. This greatly enhances the stability of lazyvec
#' implementation and should be set to TRUE during the development phase of new custom vectors
#' to avoid crashes and unexpected side-effects.
#' @param id identifier for your lazyvec, used for diagnostic output
#'
#' @return a user-defined ALTREP vector
#' @export
lazyvec <- function(metadata, vec_type, altrep_methods, package_environment = "lazyvec", diagnostics = TRUE,
  id = "lazyvec") {

  if (class(altrep_methods) != "lazyvec_api") {
    stop("Please use lazyvec_methods() to define the ALTREP methods for this vector")
  }

  # attach package
  is_attached <- require(package_environment, character.only = TRUE, quietly = TRUE)
  if (!is_attached) stop("Failed to attach package ", package_environment,
    ", please make sure it's installed correctly")

  altrep_methods_list <- altrep_methods

  # if diagnostics are active, user methods are stored in user data
  if (diagnostics) {
    altrep_methods_list <- diagnostics()
    metadata <- list(
      user_data = metadata,
      user_methods = altrep_methods,
      vec_id = id,
      vec_type = vec_type
    )
  }

  payload <- list(

    # user defined or diagnostic API
    altrep_methods_list,

    # (user) package environment in which to evaluate user defined mehods
    as.environment(paste0("package:", package_environment)),

    # user-defined metadata or diagnostic list including metadata
    metadata,

    # container for expanded vector
    NULL,

    # lazyvec package environment
    as.environment("package:lazyvec"),

    # data type
    vec_type,

    # package environment string
    package_environment
  )

  if (vec_type == "integer") {
    return(lazyvec_integer_wrapper(payload))
  }

  if (vec_type == "double") {
    return(lazyvec_real_wrapper(payload))
  }

  if (vec_type == "logical") {
    return(lazyvec_logical_wrapper(payload))
  }

  if (vec_type == "raw") {
    return(lazyvec_raw_wrapper(payload))
  }

  if (vec_type == "character") {
    return(lazyvec_string_wrapper(payload))
  }
}
