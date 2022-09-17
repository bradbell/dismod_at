// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin check_child_prior$$
$spell
	sqlite
	std
	dage
	dtime
	const
	CppAD
	struct
	nslist
$$

$section Check Priors in Child Smoothing$$

$head syntax$$
$codei%check_child_prior(
	%db%, %rate_table%, %smooth_grid%, %nslist_pair%, %prior_table%
)%$$

$head Purpose$$
Checks the
$cref/child smoothing assumptions/rate_table/child_smooth_id/$$.

$list number$$
The $cref/density_id/prior_table/density_id/$$ cannot correspond to a
$cref/laplace/density_table/density_name/laplace/$$ or
$cref/log_laplace/density_table/density_name/log_laplace/$$ density.
$lnext
The $cref/lower/prior_table/lower/$$ limit must be minus infinity
or equal to the upper limit and finite.
$lnext
The $cref/upper/prior_table/upper/$$ limit must be plus infinity
or equal to the lower limit and finite.
$lend

$head db$$
This argument has prototype
$codei%
	sqlite3* %db%
%$$
and is the database connection for $cref/logging/log_message/$$ errors.

$head rate_table$$
This argument has prototype
$codei%
	const CppAD::vector<rate_struct>& %rate_table%
%$$
and it is the
$cref/rate_table/get_rate_table/rate_table/$$.
For this table,
only the $code child_smooth_id$$ field is used.

$head smooth_grid$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_grid_struct>& %smooth_grid%
%$$
and it is the
$cref/smooth_grid/get_smooth_grid/smooth_grid/$$.
For this table, only the
$code value_prior_id$$, $code dage_prior_id$$, and $code dtime_prior_id$$
fields are used.

$head nslist_pair$$
This argument has prototype
$codei%
	const CppAD::vector<nslist_pair_struct>& %nslist_pair%
%$$
and it is the
$cref/nslist_pair/get_nslist_pair/nslist_pair/$$.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>& %prior_table%
%$$
and it is the
$cref/prior_table/get_prior_table/prior_table/$$.
For this table,
only the
$code value_prior_id$$,
$code dage_prior_id$$, and
$code dtime_prior_id$$,
field are used.

$end
*/
# include <dismod_at/check_child_prior.hpp>
# include <dismod_at/get_nslist_pair.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/null_int.hpp>
# include <cppad/utility/to_string.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_child_prior(
	sqlite3*                                 db            ,
	const CppAD::vector<rate_struct>&        rate_table    ,
	const CppAD::vector<smooth_grid_struct>& smooth_grid   ,
	const CppAD::vector<nslist_pair_struct>& nslist_pair   ,
	const CppAD::vector<prior_struct>&       prior_table   )
{	assert( rate_table.size()   == number_rate_enum );
	using std::endl;
	using std::string;
	using CppAD::to_string;
	string msg;
	//
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{
		int child_smooth_id  = rate_table[rate_id].child_smooth_id;
		int child_nslist_id  = rate_table[rate_id].child_nslist_id;
		//
		// list of child smooth_id for this rate
		CppAD::vector<int> smooth_list;
		if( child_smooth_id != DISMOD_AT_NULL_INT )
			smooth_list.push_back(child_smooth_id);
		if( child_nslist_id != DISMOD_AT_NULL_INT )
		{	assert( child_smooth_id == DISMOD_AT_NULL_INT );
			for(size_t i = 0; i < nslist_pair.size(); i++)
			{	if( nslist_pair[i].nslist_id == child_nslist_id )
				{	int smooth_id = nslist_pair[i].smooth_id;
					smooth_list.push_back( smooth_id );
				}
			}
		}
		for(size_t grid_id = 0; grid_id < smooth_grid.size(); grid_id++)
		{	bool check = false;
			for(size_t i = 0; i < smooth_list.size(); i++)
				check |= smooth_grid[grid_id].smooth_id == smooth_list[i];
			if( check)
			{	CppAD::vector<int> prior_id(3);
				CppAD::vector<string> name(3);
				prior_id[0] = smooth_grid[grid_id].value_prior_id;
				name[0]     = "child value prior:\n";
				prior_id[1] = smooth_grid[grid_id].dage_prior_id;
				name[1]     = "child dage prior:\n";
				prior_id[2] = smooth_grid[grid_id].dtime_prior_id;
				name[2]     = "child dtime prior:\n";
# ifndef NDEBUG
				double const_value = smooth_grid[grid_id].const_value;
				if( prior_id[0] == DISMOD_AT_NULL_INT )
					assert( ! std::isnan(const_value) );
				else
					assert( std::isnan(const_value) );
# endif
				for(size_t i = 0; i < 3; i++)
				if( prior_id[i] != DISMOD_AT_NULL_INT )
				{
					int    density_id = prior_table[prior_id[i]].density_id;
					double lower      = prior_table[prior_id[i]].lower;
					double upper      = prior_table[prior_id[i]].upper ;
					//
					// check for an error
					msg = "";
					//
					// value prior lower and upper equal case
					bool constant_value_prior = i == 0 && lower == upper;
					if( not constant_value_prior )
					{	if( density_id == int(laplace_enum) )
						{	msg += "density is Laplace";
						}
						if( density_id == int(log_laplace_enum) )
						{	msg += "density is Log-Laplace ";
						}
						double inf = std::numeric_limits<double>::infinity();
						if( lower != -inf  )
						{	if(msg != "" )
								msg += ", ";
							msg += "lower is not -infinity";
						}
						if( upper != inf  )
						{	if(msg != "" )
								msg += ", ";
							msg += "upper is not +infinity";
						}
					}
					if( msg != "" )
					{	size_t smooth_id = smooth_grid[grid_id].smooth_id;
						if( i == 0 )
							msg += ", and lower not equal upper";
						msg = name[i]
						+ "smooth_id = "         + to_string(smooth_id)
						+ ", smooth_grid_id = "  + to_string(grid_id)
						+ ", prior_id = "        + to_string( prior_id[i] )
						+ ": " + msg;
						string table_name  = "rate";
						error_exit(msg,  table_name, rate_id);
					}
				}
			}
		}
	}
}

} // END DISMOD_AT_NAMESPACE
