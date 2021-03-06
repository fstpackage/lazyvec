% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/lazyvec.R
\name{lazyvec}
\alias{lazyvec}
\title{Define a custom ALTREP vector}
\usage{
lazyvec(metadata, vec_type, altrep_methods,
  package_environment = "lazyvec", diagnostics = TRUE,
  id = "lazyvec")
}
\arguments{
\item{metadata}{custom metadata stored alongside the generated ALTREP vector.}

\item{vec_type}{data type required for the ALTREP vector, options are 'integer', 'double',
'logical', 'raw' and 'character'.}

\item{altrep_methods}{user-defined methods used by the resulting ALTREP vector.}

\item{package_environment}{package environment in which to evaluate the user methods. Defaults
to the lazyvec package.}

\item{diagnostics}{if TRUE, heavy type and boundary checks are performed before returning
the results of user defined methods to R. This greatly enhances the stability of lazyvec
implementation and should be set to TRUE during the development phase of new custom vectors
to avoid crashes and unexpected side-effects.}

\item{id}{identifier for your lazyvec, used for diagnostic output}
}
\value{
a user-defined ALTREP vector
}
\description{
This method creates a user-defined ALTREP vector with the complete ALTREP API created
in R instead of C/C++ sources.
}
