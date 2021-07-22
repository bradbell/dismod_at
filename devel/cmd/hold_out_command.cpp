/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <gsl/gsl_randist.h>
# include <cppad/utility/to_string.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/hold_out_command.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin hold_out_command$$
$spell
	dismod
$$

$section Hold Out Command: Randomly Sub-sample The Data$$

$head Syntax$$
$codei%dismod_at %database% hold_out %integrand_name% %max_fit%$$

$head Purpose$$
This command is used to set a maximum number of data values
that are included in subsequent fits.
It is intended to make the initialization and fitting faster.
The random sample of which values to include can be made repeatable using
$cref/random_seed/option_table/random_seed/$$.

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head integrand_name$$
This is the
$cref/integrand/integrand_table/integrand_name/$$ that we are sub-sampling.

$head max_fit$$
This is the maximum number of data points to fit for the specified integrand;
i.e., the maximum number that are not held out.
If for this integrand there are more than $icode mas_fit$$ points with
$cref/hold_out/data_table/hold_out/$$ zero in the data table,
points are randomly held out so that there are $icode max_fit$$
points fit for this integrand.

$head data_subset_table$$
Only rows of the $cref data_subset_table$$ that correspond to this integrand
are modified.
The $cref/hold_out/data_subset_table/hold_out/$$ is set one (zero)
if the corresponding data is (is not) selected for hold out.
Only points that have $icode hold_out$$ zero in the data table
can have hold_out non-zero in the data_subset table.
See the fit command $cref/hold_out/fit_command/hold_out/$$
documentation.

$head Example$$
The file $cref user_hold_out.py$$ contains an example and test
using this command.

$end
*/
void hold_out_command(
	sqlite3*                                      db                ,
	std::string&                                  integrand_name    ,
	std::string&                                  max_fit_str       ,
	const CppAD::vector<integrand_struct>&        integrand_table   ,
	const CppAD::vector<data_struct>&             data_table        )
{	using std::string;
	using CppAD::vector;
	using CppAD::to_string;
	//
	// data_subset_table
	vector<data_subset_struct> data_subset_table = get_data_subset(db);
	//
	// n_subset
	size_t n_subset = data_subset_table.size();
	//
	// max_fit
	size_t max_fit = std::atoi( max_fit_str.c_str() );
	//
	// integrand
	integrand_enum integrand = number_integrand_enum;
	for(size_t i = 0; i < size_t(number_integrand_enum); ++i)
	{	if( integrand_enum2name[i] == integrand_name )
			integrand = integrand_enum(i);
	}
	{	string msg = "hold_out_command: " + integrand_name;
		msg       += " is not a valid integrand name";
	}
	//
	// src: array of indices to choose from
	vector<int> src;
	for(size_t data_subset_id = 0; data_subset_id < n_subset; ++data_subset_id)
	{	size_t data_id      = data_subset_table[data_subset_id].data_id;
		size_t integrand_id = data_table[data_id].integrand_id;
		if( integrand ==  integrand_table[integrand_id].integrand )
		{	if( data_table[data_id].hold_out == 0 )
				src.push_back( int(data_subset_id) );
			//
			// initialize hold_out for this integrand as zero
			data_subset_table[data_subset_id].hold_out = 0;
		}
	}
	//
	// check if there will be hold outs in data_subset_table
	if( max_fit < src.size() )
	{	// rng: gsl random number generator
		gsl_rng* rng = CppAD::mixed::get_gsl_rng();
		//
		// n_choose
		size_t n_choose = src.size() - max_fit;
		//
		// dest: array of indices that are chosen
		vector<int> dest(n_choose);
		//
		// choose which elements to hold out
		gsl_ran_choose(
			rng,
			dest.data(),
			n_choose,
			src.data(),
			src.size(),
			sizeof(int)
		);
		//
		// hold out the chosen elements
		for(size_t i = 0; i < n_choose; ++i)
		{	size_t data_subset_id = dest[i];
			assert( data_subset_table[data_subset_id].hold_out == 0 );
			data_subset_table[data_subset_id].hold_out = 1;
		}
	}
	//
	// drop old data_subset table
	string sql_cmd    = "drop table data_subset";
	exec_sql_cmd(db, sql_cmd);
	//
	// write new data_subset table
	string table_name = "data_subset";
	size_t n_col      = 5;
	vector<string> col_name(n_col), col_type(n_col);
	vector<string> row_value(n_col * n_subset);
	vector<bool>   col_unique(n_col);
	//
	col_name[0]       = "data_id";
	col_type[0]       = "integer";
	col_unique[0]     = true;
	//
	col_name[1]       = "hold_out";
	col_type[1]       = "integer";
	col_unique[1]     = false;
	//
	col_name[2]       = "density_id";
	col_type[2]       = "integer";
	col_unique[2]     = false;
	//
	col_name[3]       = "eta";
	col_type[3]       = "real";
	col_unique[3]     = false;
	//
	col_name[4]       = "nu";
	col_type[4]       = "real";
	col_unique[4]     = false;
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	int data_id    = data_subset_table[subset_id].data_id;
		int hold_out   = data_subset_table[subset_id].hold_out;
		int density_id = data_subset_table[subset_id].density_id;
		double eta     = data_subset_table[subset_id].eta;
		double nu      = data_subset_table[subset_id].nu;
		row_value[n_col * subset_id + 0] = to_string( data_id );
		row_value[n_col * subset_id + 1] = to_string( hold_out );
		row_value[n_col * subset_id + 2] = to_string( density_id );
		if( std::isnan(eta) )
			row_value[n_col * subset_id + 3] = "";
		else
			row_value[n_col * subset_id + 3] = to_string( eta );
		if( std::isnan(nu) )
			row_value[n_col * subset_id + 4] = "";
		else
			row_value[n_col * subset_id + 4] = to_string( nu );
	}
	create_table(
		db, table_name, col_name, col_type, col_unique, row_value
	);
	return;
}
} // END_DISMOD_AT_NAMESPACE
