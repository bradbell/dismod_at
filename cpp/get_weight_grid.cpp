/*
$begin get_weight_grid$$
$spell
	sqlite
	struct
	cpp
	std
$$

$section C++: Get the Weight Grid Information$$
$index get, weight grid$$
$index weight, get grid$$
$index grid, get weight$$

$head Syntax$$
$codei # include <get_weight_grid>
%$$
$icode%weight_grid% = get_weight_grid(%db%)%$$

$head Purpose$$
To read the $cref weight_grid$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head weight_grid_struct$$
This is a structure with the following fields
$table
$code int weight_id$$   $pre  $$ $cnext 
	The $cref/weight_id/weight_grid/weight_id/$$ for this weighting function
$rnext
$code double age$$      $pre  $$ $cnext 
	The $cref/age/weight_grid/age/$$ for this weight value
$rnext
$code double time$$      $pre  $$ $cnext 
	The $cref/time/weight_grid/time/$$ for this weight value
$rnext
$code double weight$$    $pre  $$ $cnext 
	The $cref/weight/weight_grid/weight/$$ value
$tend        

$head weight_grid$$
The return value $icode weight_grid$$ has prototype
$codei%
	CppAD::vector<weight_grid_struct>  %weight_grid%
%$$
For each $cref/weight_grid_id/weight_grid/weight_grid_id/$$,
$codei%
	%weight_grid%[%weight_grid_id%]
%$$
is the information for a specific weighting and specific age and time.

$children%example/cpp/get_weight_grid_xam.cpp
%$$
$head Example$$
The file $cref get_weight_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cmath>
# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_weight_grid.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in weight grid";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<weight_grid_struct> get_weight_grid(sqlite3* db)
{	using std::string;

	string table_name  = "weight_grid";
	string column_name = "weight_grid_id";
	CppAD::vector<int>    weight_grid_id;
	get_table_column(db, table_name, column_name, weight_grid_id);
	size_t n_weight = weight_grid_id.size();

	column_name        =  "weight_id";
	CppAD::vector<int>     weight_id;
	get_table_column(db, table_name, column_name, weight_id);
	assert( weight_id.size() == n_weight );

	column_name        =  "age";
	CppAD::vector<double>  age;
	get_table_column(db, table_name, column_name, age);
	assert( age.size() == n_weight );

	column_name        =  "time";
	CppAD::vector<double>  time;
	get_table_column(db, table_name, column_name, time);
	assert( time.size() == n_weight );

	column_name        =  "weight";
	CppAD::vector<double>  weight;
	get_table_column(db, table_name, column_name, weight);
	assert( weight.size() == n_weight );

	CppAD::vector<weight_grid_struct> weight_grid(n_weight);
	for(size_t i = 0; i < n_weight; i++)
	{	if( weight_grid_id[i] != i )
		{	string s = 
			"weight_grid_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		weight_grid[i].weight_id = weight_id[i];
		weight_grid[i].age       = age[i];
		weight_grid[i].time      = time[i];
		weight_grid[i].weight    = weight[i];
	}
	return weight_grid;
}

} // END DISMOD_AT_NAMESPACE
