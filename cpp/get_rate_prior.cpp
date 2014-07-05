/*
$begin get_rate_prior$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Rate Prior Information$$
$index get, rate prior$$
$index rate, get prior$$
$index prior, get rate$$

$head Syntax$$
$codei%# include <get_rate_prior>
%$$
$icode%rate_prior% = get_rate_prior(%db%)%$$

$head Purpose$$
To read the $cref rate_prior$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head rate_prior_struct$$
This is a structure with the following fields
$table
$code int rate_id$$   $pre  $$ $cnext 
	The $cref/rate_id/rate_prior/rate_id/$$ for this rate prior. 
$rnext
$code int is_parent$$   $pre  $$ $cnext 
	The $cref/is_parent/rate_prior/is_parent/$$ for this rate prior. 
$rnext
$code int smooth_id$$   $pre  $$ $cnext 
	The $cref/smooth_id/rate_prior/smooth_id/$$ for this rate prior. 
$tend        

$head rate_prior$$
The return value $icode rate_prior$$ has prototype
$codei%
	CppAD::vector<rate_prior_struct>  %rate_prior%
%$$
For each $cref/rate_prior_id/rate_prior/rate_prior_id/$$,
$codei%
	%rate_prior%[%rate_prior_id%]
%$$
specifies the smoothing to use for one rate
as a parent or as a child.

$children%example/cpp/get_rate_prior_xam.cpp
%$$
$head Example$$
The file $cref get_rate_prior_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>
# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_rate_prior.hpp>
# include <dismod_at/table_error_exit.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<rate_prior_struct> get_rate_prior(sqlite3* db)
{	using std::string;

	string table_name  = "rate_prior";
	string column_name = "rate_prior_id";
	CppAD::vector<int>    rate_prior_id;
	get_table_column(db, table_name, column_name, rate_prior_id);
	size_t n_prior = rate_prior_id.size();

	column_name        =  "rate_id";
	CppAD::vector<int>     rate_id;
	get_table_column(db, table_name, column_name, rate_id);
	assert( rate_id.size() == n_prior );

	column_name        =  "is_parent";
	CppAD::vector<int>     is_parent;
	get_table_column(db, table_name, column_name, is_parent);
	assert( is_parent.size() == n_prior );

	column_name        =  "smooth_id";
	CppAD::vector<int>     smooth_id;
	get_table_column(db, table_name, column_name, smooth_id);
	assert( smooth_id.size() == n_prior );

	CppAD::vector<rate_prior_struct> rate_prior(n_prior);
	for(size_t i = 0; i < n_prior; i++)
	{	if( rate_prior_id[i] != i )
		{	string s = 
			"rate_prior_id must start at zero and increment by one.";
			table_error_exit("rate_prior", i, s);
		}
		rate_prior[i].rate_id     = rate_id[i];
		rate_prior[i].is_parent   = is_parent[i];
		rate_prior[i].smooth_id   = smooth_id[i];
	}
	return rate_prior;
}

} // END DISMOD_AT_NAMESPACE
