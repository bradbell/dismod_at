// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_integrand_table$$
$spell
   Sincidence
   Tincidence
   struct
   sqlite
   enum
   cpp
   mtexcess
   mtother
   mtwith
   mtall
   mtstandard
   relrisk
   mtspecific
   cv
   mulcov
   const
   covariate
   integrands
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++: Get the Integrand Table Information$$

$head Syntax$$
$icode%integrand_table% = get_integrand_table(
   %db%, %mulcov_table%, %option_table%
)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Purpose$$
To read the $cref integrand_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
is an open connection to the database.

$head mulcov_table$$
is the $cref/mulcov_table/get_mulcov_table/$$
(used to check integrands that are covariate multipliers).

$head option_table$$
is the $cref/option_table/get_option_table/$$
(used to determine if $cref/rate_case/option_table/rate_case/no_ode/$$
is $code no_ode$$).

$head integrand_table$$
For each $cref/integrand_id/integrand_table/integrand_id/$$,
$codei%
   %integrand_table%[%integrand_id%]
%$$
is the information for the corresponding integrand.

$head integrand_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code integrand_enum$$ $cnext
   $code integrand$$ $cnext
   An enum type for this integrand; see below
$rnext
$code double$$ $cnext
   $cref/minimum_meas_cv/integrand_table/minimum_meas_cv/$$ $cnext
   minimum measurement coefficient of variation for this integrand
$rnext
$code int$$ $cnext
   $cref/mulcov_id/mulcov_table/mulcov_id/$$ $cnext
   covariate multiplier corresponding to this integrand
   ($code DISMOD_AT_NULL_INT$$ if not a covariate multiplier).
$tend

$head integrand_enum$$
This is an enum type with the following values:
$table
$icode integrand_enum$$  $pre  $$ $cnext $icode integrand_name$$ $rnext
$code Sincidence_enum$$  $pre  $$ $cnext $code Sincidence$$      $rnext
$code Tincidence_enum$$  $pre  $$ $cnext $code Tincidence$$      $rnext
$code remission_enum$$   $pre  $$ $cnext $code remission$$       $rnext
$code mtexcess_enum$$    $pre  $$ $cnext $code mtexcess$$        $rnext
$code mtother_enum$$     $pre  $$ $cnext $code mtother$$         $rnext
$code mtwith_enum$$      $pre  $$ $cnext $code mtwith$$          $rnext
$code susceptible_enum$$ $pre  $$ $cnext $code susceptible$$     $rnext
$code withC_enum$$       $pre  $$ $cnext $code withC$$           $rnext
$code prevalence_enum$$  $pre  $$ $cnext $code prevalence$$      $rnext
$code mtspecific_enum$$  $pre  $$ $cnext $code mtspecific$$      $rnext
$code mtall_enum$$       $pre  $$ $cnext $code mtall$$           $rnext
$code mtstandard_enum$$  $pre  $$ $cnext $code mtstandard$$      $rnext
$code relrisk_enum$$     $pre  $$ $cnext $code relrisk$$         $rnext
$code mulcov_enum$$      $pre  $$ $cnext $codei%mulcov_%mulcov_id%$$
$tend

$head integrand_enum2name$$
This is a global variable.
If $icode%integrand%$$, is an $code integrand_enum$$ value,
$codei%integrand_enum2name[%integrand%]%$$ is the
$icode integrand_name$$ corresponding to the enum value.
The mulcov case is special because
$codei%
   integrand_enum2name[mulcov_enum] == "mulcov"
%$$

$children%example/devel/table/get_integrand_table_xam.cpp
%$$
$head Example$$
The file $cref get_integrand_table_xam.cpp$$ contains an example that uses
this function.

$end
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
