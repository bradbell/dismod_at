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
$codei%# include <get_integrand_table>
%$$
$icode%integrand_table% = get_integrand_table(%db%)%$$

$head Purpose$$
To read the $cref integrand_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
	sqlite3* %db%
%$$
and is an open connection to the database.

$head integrand_enum$$
This is an enum type with the following values:
$code incidence_enum$$,
$code remission_enum$$,
$code mtexcess_enum$$,
$code mtother_enum$$,
$code mtwith_enum$$,
$code prevalence_enum$$,
$code mtspecific_enum$$,
$code mtall_enum$$,
$code mtstandard_enum$$,
$code relrisk_enum$$.

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


$children%example/cpp/get_integrand_table_xam.cpp
%$$
$head Example$$
The file $cref get_integrand_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <cppad/vector.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/get_integrand_table.hpp>

namespace {
	void error_exit(size_t row, std::string msg)
	{	using std::cerr;
		using std::endl;
		cerr << msg << endl;
		cerr << "Error detected in integrand table";
		if( row > 0 )
			cerr << " at row " << row;
		cerr << "." << endl;
		exit(1);
	}
} 

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<integrand_enum> get_integrand_table(sqlite3* db)
{	using std::string;

	string table_name  = "integrand";
	string column_name = "integrand_id";
	CppAD::vector<int>  integrand_id;
	get_table_column(db, table_name, column_name, integrand_id);
	
	column_name        =  "integrand_name";
	CppAD::vector<string>  integrand_name;
	get_table_column(db, table_name, column_name, integrand_name);
	assert( integrand_id.size() == integrand_name.size() );

	integrand_enum enum_list[] = {
		incidence_enum,
		remission_enum,
		mtexcess_enum,
		mtother_enum,
		mtwith_enum,
		prevalence_enum,
		mtspecific_enum,
		mtall_enum,
		mtstandard_enum,
		relrisk_enum
	};
	const char* name_list[] = {
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
	size_t n_list = sizeof(enum_list) / sizeof(enum_list[0]);
	
	size_t n_out = integrand_name.size();
	CppAD::vector<integrand_enum> integrand_table(n_out);
	size_t n_integrand = integrand_id.size();
	for(size_t i = 0; i < n_out; i++)
	{	if( integrand_id[i] != i )
		{	string s = "integrand_id must start at zero and increment by one.";
			error_exit(i+1, s);
		}
		bool found = false;
		for(size_t j = 0; j < n_list; j++)
		{	if( integrand_name[i] == name_list[j] )
			{	integrand_table[i] = enum_list[j];
				found = true;
			}
		}
		if( ! found )
		{	string s = "integrand_name is not a valid choice.";
			error_exit(i+1, s);
		}
	}
	return integrand_table;
}

} // END DISMOD_AT_NAMESPACE
