// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
$$

$section C++: Get the Integrand Table Information$$

$head Syntax$$
$icode%integrand_table% = get_integrand_table(%db%, %n_mulcov%)%$$

$head Purpose$$
To read the $cref integrand_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head n_mulcov$$
This argument has prototype
$codei%
	size_t %n_mulcov%
%$$
and is the size of the $cref/mulcov_table/get_mulcov_table/$$.

$head integrand_table$$
The return value $icode integrand_table$$ has prototype
$codei%
	CppAD::vector<integrand_struct>  %integrand_table%
%$$
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
$code integrand_enum$$ $cnext $code integrand$$ $cnext
	An enum type for this integrand; see below
$rnext
$code double$$ $cnext $cnext
	$cref/minimum_meas_cv/integrand_table/minimum_meas_cv/$$ $cnext
	minimum measurement coefficient of variation for this integrand
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
$code relrisk_enum$$     $pre  $$ $cnext $code relrisk$$
$tend


$head integrand_enum2name$$
This is a global variable.
If $icode%integrand%$$, is an $code integrand_enum$$ value,
$codei%integrand_enum2name[%integrand%]%$$ is the
$icode integrand_name$$ corresponding to the enum value.

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

// rate names in same order as enum type in get_integrand_table.hpp
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
CppAD::vector<integrand_struct> get_integrand_table(
	sqlite3* db       ,
	size_t   n_mulcov
)
{	using std::string;

	// check for minimum_cv column
	string sql_cmd = "pragma table_info(integrand)";
	string result  = exec_sql_cmd(db, sql_cmd, ',');
	if( result.find(",minimum_meas_cv,") == string::npos )
	{	string msg = "column minimum_meas_cv not in integrand table\n";
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
	{	// integrand
		integrand_enum integrand = number_integrand_enum;
		if( std::strncmp(integrand_name[integrand_id].c_str(), "mulcov_", 7) )
			integrand = mulcov_enum;
		for(size_t j = 0; j < number_integrand_enum; j++)
		{	if( integrand_name[integrand_id] == integrand_enum2name[j] )
				integrand = integrand_enum(j);
			if( integrand_name[integrand_id] == "mulcov" )
				integrand = number_integrand_enum;
		}
		if( integrand == number_integrand_enum )
		{	string msg = integrand_name[integrand_id];
			msg       += " is not a valid choice for integrand_name.";
			error_exit(msg, table_name, integrand_id);
		}
		integrand_table[integrand_id].integrand = integrand;
		//
		// minumum_meas_cv
		if( integrand_table[integrand_id].minimum_meas_cv < 0.0 )
		{	string msg = integrand_name[integrand_id];
			msg       += " minimum_meas_cv < 0.0";
			error_exit(msg, table_name, integrand_id);
		}
		if( integrand_table[integrand_id].minimum_meas_cv > 1.0 )
		{	string msg = integrand_name[integrand_id];
			msg       += " minimum_meas_cv > 1.0";
			error_exit(msg, table_name, integrand_id);
		}
		integrand_table[integrand_id].minimum_meas_cv =
			minimum_meas_cv[integrand_id];
		integrand_table[integrand_id].mulcov_id = DISMOD_AT_NULL_INT;
		if( integrand == mulcov_enum )
		{	string mulcov_id_str = integrand_name[integrand_id].substr(7);
			int    mulcov_id     = std::atoi(mulcov_id_str.c_str() );
			if( n_mulcov <= size_t(mulcov_id) )
			{	string msg = integrand_name[integrand_id];
				msg       += " mulcov_id not non-negative and less than ";
				msg       += CppAD::to_string(n_mulcov);
				error_exit(msg, table_name, integrand_id);
			}
			integrand_table[integrand_id].mulcov_id = mulcov_id;
		}
	}
	return integrand_table;
}

} // END DISMOD_AT_NAMESPACE
