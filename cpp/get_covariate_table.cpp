/*
$begin get_covariate_table$$
$spell
	sqlite
	struct
	cpp
	std
	covariate
$$

$section C++: Get the Covariate Table Information$$
$index get, covariate table$$
$index covariate, get table$$
$index table, get covariate$$

$head Syntax$$
$codei%# include <get_covariate_table>
%$$
$icode%covariate_table% = get_covariate_table(%db%)%$$

$head Purpose$$
To read the $cref covariate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head covariate_struct$$
This is a structure with the following fields
$table
$code std::string name$$   $pre  $$ $cnext 
	The $cref/covariate_name/covariate_table/covariate_name/$$ for this covariate  $rnext
$code double reference$$         $pre  $$ $cnext 
	The $cref/reference/covariate_table/reference/$$ 
	value for this covariate
$tend        

$head covariate_table$$
The return value $icode covariate_table$$ has prototype
$codei%
	CppAD::vector<covariate_struct>  %covariate_table%
%$$
For each $cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
	%covariate_table%[%covariate_id%]
%$$
is the information for the corresponding covariate.

$children%example/cpp/get_covariate_table_xam.cpp
%$$
$head Example$$
The file $cref get_covariate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/table_error_exit.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<covariate_struct> get_covariate_table(sqlite3* db)
{	using std::string;

	string table_name  = "covariate";
	string column_name = "covariate_id";
	CppAD::vector<int>    covariate_id;
	get_table_column(db, table_name, column_name, covariate_id);
	size_t n_covariate = covariate_id.size();

	column_name        =  "covariate_name";
	CppAD::vector<string>  covariate_name;
	get_table_column(db, table_name, column_name, covariate_name);
	assert( n_covariate == covariate_name.size() );

	column_name           =  "reference";
	CppAD::vector<double>     reference;
	get_table_column(db, table_name, column_name, reference);
	assert( n_covariate == reference.size() );

	CppAD::vector<covariate_struct> covariate_table(n_covariate);
	for(size_t i = 0; i < n_covariate; i++)
	{	if( covariate_id[i] != i )
		{	string s = "covariate_id must start at zero and increment by one.";
			table_error_exit("covariate", i, s);
		}
		covariate_table[i].name      = covariate_name[i];
		covariate_table[i].reference = reference[i];
	}
	return covariate_table;
}

} // END DISMOD_AT_NAMESPACE
