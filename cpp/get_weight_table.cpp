/*
$begin get_weight_table$$
$spell
	sqlite
	enum
	cpp
	std
$$

$section C++: Get the Weight Table Information$$
$index get, weight table$$
$index weight, get table$$
$index table, get weight$$

$head Syntax$$
$codei # include <get_weight_table>
%$$
$icode%weight_table% = get_weight_table(%db%)%$$

$head Purpose$$
To read the $cref weight_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head weight_table$$
The return value $icode weight_table$$ has prototype
$codei%
	CppAD::vector<std::string>  %weight_table%
%$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%weight_table%[%weight_id%]
%$$
is the string for the corresponding
$cref/weight_name/weight_table/weight_name/$$.

$comment%example/cpp/get_weight_grid_xam.cpp in included by weight_grid.omh
%$$
$head Example$$
The file $cref get_weight_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/table_error_exit.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<std::string> get_weight_table(sqlite3* db)
{	using std::string;

	string table_name  = "weight";
	string column_name = "weight_id";
	CppAD::vector<int>    weight_id;
	get_table_column(db, table_name, column_name, weight_id);
	size_t n_weight = weight_id.size();
	
	column_name        =  "weight_name";
	CppAD::vector<string>  weight_name;
	get_table_column(db, table_name, column_name, weight_name);
	assert( weight_name.size() == n_weight );

	for(size_t i = 0; i < n_weight; i++)
	{	if( weight_id[i] != i )
		{	string s = "weight_id must start at zero and increment by one.";
			table_error_exit("weight", i, s);
		}
	}
	return weight_name;
}

} // END DISMOD_AT_NAMESPACE
