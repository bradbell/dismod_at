/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
         GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <gsl/gsl_randist.h>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/hold_out_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/create_table.hpp>
# include <dismod_at/get_integrand_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
$begin hold_out_command$$
$spell
	sim
	dismod
	var
$$

$section The Hold Out Command$$

$head Syntax$$
$codei%dismod_at %database% hold_out %integrand_name% %max_fit%$$

$head database$$
Is an
$href%http://www.sqlite.org/sqlite/%$$ database containing the
$code dismod_at$$ $cref input$$ tables which are not modified.

$head integrand_name$$
This is the
$cref/integrand/integrand_table/integrand_name/$$ that we are sub-sampling.

$head max_fit$$
This is the maximum number of data points to fit for the specified integrand.
If there are more than this number of points,
points will be randomly held out so that there are $icode max_fit$$
points fit for this integrand.

$head data_subset_table$$
Only rows of the $cref data_subset_table$$ that correspond to this integrand
are modified.
The $cref/hold_out/data_subset_table/hold_out/$$ is set one (zero)
if the corresponding data is (is not) selected for hold out.

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
	const CppAD::vector<data_struct>&             data_table        ,
	CppAD::vector<data_subset_struct>&            data_subset_table )
{	using std::string;
	using CppAD::vector;
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
	vector<size_t> src;
	for(size_t data_subset_id = 0; data_subset_id < n_subset; ++data_subset_id)
	{	size_t data_id                = data_subset_table[data_subset_id].data_id;
		size_t integrand_id           = data_table[data_id].integrand_id;
		if( integrand ==  integrand_table[integrand_id].integrand )
			src.push_back(data_subset_id);
		//
		// initialize hold_out for this integrand as zero
		data_subset_table[data_subset_id].hold_out = 0;
	}
	//
	// check if no hold outs in data_subset_table
	if( src.size() <= max_fit )
		return;
	//
	// rng: gsl random number generator
	gsl_rng* rng = CppAD::mixed::get_gsl_rng();
	//
	// n_choose
	size_t n_choose = src.size() - max_fit;
	//
	// dest: array of indices that are chosen
	vector<int> dest(n_choose);
	//
	// choose which elements to hold out
	gsl_ran_choose(rng, dest.data(), n_choose, src.data(), src.size(), sizeof(int));
	//
	// hold out the chosen elements
	for(size_t i = 0; i < n_choose; ++i)
	{	size_t data_subset_id = dest[i];
		assert( data_subset_table[data_subset_id].hold_out == 0 );
		data_subset_table[data_subset_id].hold_out = 1;
	}
	return;
}
} // END_DISMOD_AT_NAMESPACE
