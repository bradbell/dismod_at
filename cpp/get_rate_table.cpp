/*
$begin get_rate_table$$
$spell
	sqlite
	enum
	cpp
$$

$section Get the Rate Table Information$$

$head Syntax$$
$codei # include <get_rate_table>
%$$
$icode%rate_table% = get_rate_table(%db%)%$$

$head Purpose$$
To read the $cref rate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head rate_enum$$
This is an enum type with the following values:
$code iota_enum$$,
$code rho_enum$$,
$code chi_enum$$,
$code omega_enum$$.

$head rate_table$$
The return value $icode rate_table$$ has prototype
$codei%
	CppAD::vector<rate_enum>  %rate_table%
%$$
For each $cref/rate_id/rate_table/rate_id/$$,
$codei%
	%rate_table%[%rate_id%]
%$$
is the enum value for the corresponding
$cref/rate_name/rate_table/rate_name/$$.


$children%example/cpp/get_rate_table_xam.cpp
%$$
$head Example$$
The file $cref get_rate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_rate_table.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in rate table";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
}

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<rate_enum> get_rate_table(sqlite3* db)
{	using std::string;

	string table_name  = "rate";
	string column_name = "rate_id";
	CppAD::vector<int>  rate_id;
	get_table_column(db, table_name, column_name, rate_id);
	
	column_name        =  "rate_name";
	CppAD::vector<string>  rate_name;
	get_table_column(db, table_name, column_name, rate_name);

	if( rate_id.size() != size_t( number_rate_enum ) )
	{	std::stringstream ss;
		ss << "rate table does not have ";
		ss << size_t( number_rate_enum) << " rows.";
		error_exit(0, ss.str());
	}
	assert( rate_id.size() == rate_name.size() );

	CppAD::vector<rate_enum> rate_table(number_rate_enum);
	for(size_t i = 0; i < number_rate_enum; i++)
	{	if( rate_id[i] != i )
		{	string s = "rate_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		if( rate_name[i] == "iota" )
			rate_table[i] = iota_enum;
		else if( rate_name[i] == "rho" )
			rate_table[i] = rho_enum;
		else if( rate_name[i] == "chi" )
			rate_table[i] = chi_enum;
		else if( rate_name[i] == "omega" )
			rate_table[i] = omega_enum;
		else
		{	string s = "rate_name is not iota, rho, chi, or omega.";
			error_exit(i+1, s);
		}
	}
	return rate_table;
}

} // END DISMOD_AT_NAMESPACE
