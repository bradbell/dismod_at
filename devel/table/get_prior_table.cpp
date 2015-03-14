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
$begin get_prior_table$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Prior Table Information$$
$index get, prior table$$
$index prior, get table$$
$index table, get prior$$

$head Syntax$$
$icode%prior_table% = get_prior_table(%db%)%$$

$head Purpose$$
To read the $cref prior_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head prior_table$$
The return value $icode prior_table$$ has prototype
$codei%
	CppAD::vector<prior_struct> %prior_table%
%$$
For each $cref/prior_id/prior_table/prior_id/$$,
$codei%
	%prior_table%[%prior_id%]
%$$
is the information for the corresponding prior.

$head prior_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code prior_name$$   $cnext 
	The $cref/prior_name/prior_table/prior_name/$$ for this prior  
$rnext
$code int$$ $cnext $code density_id$$  $cnext 
	The $cref/density_id/prior_table/density_id/$$ for this prior 
$rnext
$code double$$ $cnext $code lower$$ $cnext
	The $cref/lower/prior_table/lower/$$ limit for this prior  
$rnext
$code double$$ $cnext $code upper$$         $cnext
	The $cref/upper/prior_table/upper/$$ limit for this prior  $rnext
$code double$$ $cnext $code mean$$          $cnext
	The $cref/mean/prior_table/mean/$$ for this prior  $rnext
$code double$$ $cnext $code std$$           $cnext
	The $cref/std/prior_table/std/$$ for this prior  $rnext
$code double$$ $cnext $code eta$$           $cnext
	The $cref/eta/prior_table/eta/$$ for this prior 
$tend        

$children%example/devel/table/get_prior_table_xam.cpp
%$$
$head Example$$
The file $cref get_prior_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<prior_struct> get_prior_table(sqlite3* db)
{	using std::string;

	string table_name  = "prior";
	size_t n_prior      = check_table_id(db, table_name);

	string column_name =  "prior_name";
	CppAD::vector<string>  prior_name;
	get_table_column(db, table_name, column_name, prior_name);
	assert( prior_name.size() == n_prior );

	column_name        =  "density_id";
	CppAD::vector<int>     density_id;
	get_table_column(db, table_name, column_name, density_id);
	assert( density_id.size() == n_prior );

	column_name        =  "lower";
	CppAD::vector<double>  lower;
	get_table_column(db, table_name, column_name, lower);
	assert( lower.size() == n_prior );

	column_name        =  "upper";
	CppAD::vector<double>  upper;
	get_table_column(db, table_name, column_name, upper);
	assert( upper.size() == n_prior );

	column_name        =  "mean";
	CppAD::vector<double>  mean;
	get_table_column(db, table_name, column_name, mean);
	assert( mean.size() == n_prior );

	column_name        =  "std";
	CppAD::vector<double>  std;
	get_table_column(db, table_name, column_name, std);
	assert( std.size() == n_prior );

	column_name        =  "eta";
	CppAD::vector<double>  eta;
	get_table_column(db, table_name, column_name, eta);
	assert( eta.size() == n_prior );

	double minus_infinity = std::atof("-inf");
	double plus_infinity  = std::atof("+inf");
	CppAD::vector<prior_struct> prior_table(n_prior);
	for(size_t i = 0; i < n_prior; i++)
	{	prior_table[i].prior_name  = prior_name[i];
		prior_table[i].density_id = density_id[i];
		//
		prior_table[i].lower      = lower[i];
		if( std::isnan( lower[i] ) )
			prior_table[i].lower      = minus_infinity;
		//
		prior_table[i].upper      = upper[i];
		if( std::isnan( upper[i] ) )
			prior_table[i].upper      = plus_infinity;
		//
		prior_table[i].mean       = mean[i];
		prior_table[i].std        = std[i];
		prior_table[i].eta        = eta[i];
	}
	return prior_table;
}

} // END DISMOD_AT_NAMESPACE
