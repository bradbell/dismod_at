// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_integrand_table}
{xrst_spell
   enum
   mtall
   mtexcess
   mtother
   mtspecific
   mtstandard
   mtwith
   relrisk
   tincidence
}

C++: Get the Integrand Table Information
########################################

Syntax
******

| *integrand_table* = ``get_integrand_table`` (
| |tab| *db* , *mulcov_table* , *option_table*
| )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
To read the :ref:`integrand_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype
is an open connection to the database.

mulcov_table
************
is the :ref:`mulcov_table<get_mulcov_table-name>`
(used to check integrands that are covariate multipliers).

option_table
************
is the :ref:`option_table<get_option_table-name>`
(used to determine if :ref:`rate_case<option_table@rate_case@no_ode>`
is ``no_ode`` ).

integrand_table
***************
For each :ref:`integrand_table@integrand_id` ,

   *integrand_table* [ *integrand_id* ]

is the information for the corresponding integrand.

integrand_struct
****************
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``integrand_enum``
     - ``integrand``
     - An enum type for this integrand; see below
   * - ``double``
     - :ref:`integrand_table@minimum_meas_cv`
     - minimum measurement coefficient of variation for this integrand
   * - ``int``
     - :ref:`mulcov_table@mulcov_id`
     - covariate multiplier corresponding to this integrand
       (``DISMOD_AT_NULL_INT`` if not a covariate multiplier).

integrand_enum
**************
This is an enum type with the following values:

.. csv-table::
   :widths: auto

   *integrand_enum*,*integrand_name*
   ``Sincidence_enum``,``Sincidence``
   ``Tincidence_enum``,``Tincidence``
   ``remission_enum``,``remission``
   ``mtexcess_enum``,``mtexcess``
   ``mtother_enum``,``mtother``
   ``mtwith_enum``,``mtwith``
   ``susceptible_enum``,``susceptible``
   ``withC_enum``,``withC``
   ``prevalence_enum``,``prevalence``
   ``mtspecific_enum``,``mtspecific``
   ``mtall_enum``,``mtall``
   ``mtstandard_enum``,``mtstandard``
   ``relrisk_enum``,``relrisk``
   ``mulcov_enum``,``mulcov_`` *mulcov_id*

integrand_enum2name
*******************
This is a global variable.
If *integrand* , is an ``integrand_enum`` value,
``integrand_enum2name`` [ *integrand* ] is the
*integrand_name* corresponding to the enum value.
The mulcov case is special because

   ``integrand_enum2name`` [ ``mulcov_enum`` ] == ``"mulcov"``

{xrst_toc_hidden
   example/devel/table/get_integrand_table_xam.cpp
}
Example
*******
The file :ref:`get_integrand_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_integrand_table}
-----------------------------------------------------------------------------
*/



# include <cstring>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// integrand names in same order as enum type in get_integrand_table.hpp
const char* integrand_enum2name[] = {
   "Sincidence",
   "Tincidence",
   "remission",
   "mtexcess",
   "mtother",
   "mtwith",
   "susceptible",
   "withC",
   "prevalence",
   "mtspecific",
   "mtall",
   "mtstandard",
   "relrisk",
   "mulcov"
};
// BEGIN_PROTOTYPE
CppAD::vector<integrand_struct> get_integrand_table(
   sqlite3*                            db           ,
   const CppAD::vector<mulcov_struct>& mulcov_table ,
   const CppAD::vector<option_struct>& option_table
)
// END_PROTOTYPE
{  using std::string;

   // determine if rate_case is no_ode
   bool no_ode = false;
   for(size_t option_id = 0; option_id < option_table.size(); ++option_id)
   {  if( option_table[option_id].option_name == "rate_case" )
      {  no_ode = option_table[option_id].option_value == "no_ode";
      }
   }

   // check for minimum_cv column
   string sql_cmd = "pragma table_info(integrand)";
   string result  = exec_sql_cmd(db, sql_cmd, ',');
   if( result.find(",minimum_meas_cv,") == string::npos )
   {  string msg = "column minimum_meas_cv not in integrand table\n";
      msg       += "it was added to this table on 2018-05-23.";
      error_exit(msg);
   }

   string table_name  = "integrand";
   size_t n_integrand = check_table_id(db, table_name);

   string column_name =  "integrand_name";
   CppAD::vector<string>  integrand_name;
   get_table_column(db, table_name, column_name, integrand_name);
   assert( n_integrand == integrand_name.size() );

   column_name = "minimum_meas_cv";
   CppAD::vector<double> minimum_meas_cv;
   get_table_column(db, table_name, column_name, minimum_meas_cv);
   assert( n_integrand == minimum_meas_cv.size() );

   CppAD::vector<integrand_struct> integrand_table(n_integrand);
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   {  // integrand
      string name              = integrand_name[integrand_id];
      integrand_enum integrand = number_integrand_enum;
      if( std::strncmp(name.c_str(), "mulcov_", 7) == 0 )
         integrand = mulcov_enum;
      for(size_t j = 0; j < number_integrand_enum; j++)
      {  if( name == integrand_enum2name[j] )
            integrand = integrand_enum(j);
         // mulcov without _# is not a valid integrand name
         if( name == "mulcov" )
            integrand = number_integrand_enum;
      }
      if( integrand == number_integrand_enum )
      {  string msg = name;
         msg       += " is not a valid choice for integrand_name.";
         error_exit(msg, table_name, integrand_id);
      }
      switch( integrand )
      {  // valid no_ode cases
         case Sincidence_enum:
         case Tincidence_enum:
         case remission_enum:
         case mtexcess_enum:
         case mtother_enum:
         case mtwith_enum:
         case relrisk_enum:
         case mulcov_enum:
         break;

         default:
         if( no_ode )
         {  string msg = name + " is not a valid integrand because";
            msg += " rate_case in the option table is no_ode";
            error_exit(msg, table_name, integrand_id);
         }
         break;
      }
      //
      integrand_table[integrand_id].integrand = integrand;
      //
      // minumum_meas_cv
      if( integrand_table[integrand_id].minimum_meas_cv < 0.0 )
      {  string msg = name + " minimum_meas_cv < 0.0";
         error_exit(msg, table_name, integrand_id);
      }
      if( integrand_table[integrand_id].minimum_meas_cv > 1.0 )
      {  string msg = name + " minimum_meas_cv > 1.0";
         error_exit(msg, table_name, integrand_id);
      }
      integrand_table[integrand_id].minimum_meas_cv =
         minimum_meas_cv[integrand_id];
      integrand_table[integrand_id].mulcov_id = DISMOD_AT_NULL_INT;
      if( integrand == mulcov_enum )
      {  string mulcov_id_str = name.substr(7);
         int    mulcov_id     = std::atoi(mulcov_id_str.c_str() );
         //
         // make sure that mulcov_id is valid
         if( mulcov_table.size() <= size_t(mulcov_id) )
         {  string msg = name;
            msg       += " mulcov_id is greater or equal number of ";
            msg       += "entries  in mulcov table";
            error_exit(msg, table_name, integrand_id);
         }
         //
         for(size_t i = 0; i < mulcov_table.size(); ++i)
         if( size_t(mulcov_table[i].integrand_id) == integrand_id )
         {  string msg = "integrand_id ";
            msg += CppAD::to_string(integrand_id);
            msg += " corresponds to the covariate multiplier " + name;
            error_exit(msg, "mulcov", i);
         }
         integrand_table[integrand_id].mulcov_id = mulcov_id;
      }
   }
   return integrand_table;
}

} // END DISMOD_AT_NAMESPACE
