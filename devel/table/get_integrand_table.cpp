// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin get_integrand_table$$
$spell
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
$$

$section C++: Get the Integrand Table Information$$
$index get, integrand table$$
$index integrand, get table$$
$index table, get integrand$$

$head Syntax$$
$icode%integrand_table% = get_integrand_table(%db%)%$$

$head Purpose$$
To read the $cref integrand_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head integrand_table$$
The return value $icode integrand_table$$ has prototype
$codei%
	CppAD::vector<integrand_enum>  %integrand_table%
%$$
For each $cref/integrand_id/integrand_table/integrand_id/$$,
$codei%
	%integrand_table%[%integrand_id%]
%$$
is the enum value for the corresponding
$cref/integrand_name/integrand_table/integrand_name/$$.

$head integrand_enum$$
This is an enum type with the following values:
$table
$icode integrand$$      $pre  $$ $cnext Corresponding String    $rnext
$code incidence_enum$$  $pre  $$ $cnext $code "incidence"$$     $rnext
$code remission_enum$$  $pre  $$ $cnext $code "remission"$$     $rnext
$code mtexcess_enum$$   $pre  $$ $cnext $code "mtexcess"$$      $rnext
$code mtother_enum$$    $pre  $$ $cnext $code "mtother"$$       $rnext
$code mtwith_enum$$     $pre  $$ $cnext $code "mtwith"$$        $rnext
$code prevalence_enum$$ $pre  $$ $cnext $code "prevalence"$$    $rnext
$code mtspecific_enum$$ $pre  $$ $cnext $code "mtspecific"$$    $rnext
$code mtall_enum$$      $pre  $$ $cnext $code "mtall"$$         $rnext
$code mtstandard_enum$$ $pre  $$ $cnext $code "mtstandard"$$    $rnext
$code relrisk_enum$$    $pre  $$ $cnext $code "relrisk"$$
$tend

$head integrand_enum2name$$
This is a global variable. If $icode%integrand% < number_integrand_enum%$$, 
$codei%integrand_enum2name[%integrand%]%$$ is the string corresponding
to the $icode integrand$$ enum value.


$children%example/devel/table/get_integrand_table_xam.cpp
%$$
$head Example$$
The file $cref get_integrand_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <dismod_at/dismod_at.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// rate names in same order as enum type in get_integrand_table.hpp
const char* integrand_enum2name[] = {
	"incidence",
	"remission",
	"mtexcess",
	"mtother",
	"mtwith",
	"prevalence",
	"mtspecific",
	"mtall",
	"mtstandard",
	"relrisk"
};
CppAD::vector<integrand_enum> get_integrand_table(sqlite3* db)
{	using std::string;

	string table_name  = "integrand";
	size_t n_integrand = check_table_id(db, table_name);
	
	string column_name =  "integrand_name";
	CppAD::vector<string>  integrand_name;
	get_table_column(db, table_name, column_name, integrand_name);
	assert( n_integrand == integrand_name.size() );

	CppAD::vector<integrand_enum> integrand_table(n_integrand);
	for(size_t i = 0; i < n_integrand; i++)
	{	bool found = false;
		for(size_t j = 0; j < number_integrand_enum; j++)
		{	if( integrand_name[i] == integrand_enum2name[j] )
			{	integrand_table[i] = integrand_enum(j);
				found = true;
			}
		}
		if( ! found )
		{	string s = "integrand_name is not a valid choice.";
			table_error_exit("integrand", i, s);
		}
	}
	return integrand_table;
}

} // END DISMOD_AT_NAMESPACE
