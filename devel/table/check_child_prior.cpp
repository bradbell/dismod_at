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
$begin check_child_prior$$
$spell
	std
	dage
	dtime
	const
	CppAD
	struct
$$

$section Check Priors in Child Smoothing$$

$head syntax$$
$codei%check_child_prior(%rate_table%, %smooth_grid%, %prior_table%)%$$

$head Purpose$$
Checks the 
$cref/child smoothing assumptions/rate_table/child_smooth_id/$$.
Note that 
$cref/dage_prior_id/smooth_grid_table/dage_prior_id/$$ 
for the last age point, and
$cref/dtime_prior_id/smooth_grid_table/dtime_prior_id/$$ 
for the last time point,
are $code -1$$ and there is no prior to check for these cases.

$list number$$
The $cref/density_id/prior_table/density_id/$$ must correspond
to a $code gaussian$$ density.
$lnext
The $cref/mean/prior_table/mean/$$ must be zero.
$lnext
The $cref/std/prior_table/std/$$ must be greater than zero.
$lnext
The $cref/lower/prior_table/lower/$$ limit must be minus infinity.
$lnext
The $cref/upper/prior_table/upper/$$ limit must be plus infinity.
$lend

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
# include <dismod_at/include/check_child_prior.hpp>
# include <dismod_at/include/get_density_table.hpp>
# include <dismod_at/include/table_error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void check_child_prior(
	const CppAD::vector<rate_struct>&        rate_table    ,
	const CppAD::vector<smooth_grid_struct>& smooth_grid   ,
	const CppAD::vector<prior_struct>&       prior_table   )
{	assert( rate_table.size()   == number_rate_enum );
	std::stringstream msg;
	using std::endl;
	//
	for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
	{
		int child_smooth_id  = rate_table[rate_id].child_smooth_id;
		for(size_t grid_id = 0; grid_id < smooth_grid.size(); grid_id++)
		if( smooth_grid[grid_id].smooth_id == child_smooth_id )
		{	CppAD::vector<int> prior_id(3);
			CppAD::vector<std::string> name(3);
			prior_id[0] = smooth_grid[grid_id].value_prior_id;
			name[0]     = "child value prior";
			prior_id[1] = smooth_grid[grid_id].dage_prior_id;
			name[1]     = "child dage prior";
			prior_id[2] = smooth_grid[grid_id].dtime_prior_id;
			name[2]     = "child dtime prior";
			// skip dage and dtime priors for last age and last time
			for(size_t i = 0; i < 3; i++) if( prior_id[i] != -1 )
			{	int    density_id = prior_table[prior_id[i]].density_id; 
				double mean       = prior_table[prior_id[i]].mean; 
				double std        = prior_table[prior_id[i]].std; 
				double lower      = prior_table[prior_id[i]].lower;
				double upper      = prior_table[prior_id[i]].upper ;
				bool   ok         = true;
				if( density_id != int( gaussian_enum ) )
				{	msg << name[i] << " density not gaussian" << endl;
					ok = false;
				}
				if( mean != 0.0 )
				{	msg << name[i] << " mean not zero" << endl;
					ok = false;
				}
				if( std <= 0.0 )
				{	msg << name[i] << " std not greater than zero" << endl;
					ok = false;
				}
				double inf = std::numeric_limits<double>::infinity();
				if( lower != -inf )
				{	msg << name[i] << " lower not minus infinity" << endl;
					ok = false;
				}
				if( upper != inf )
				{	msg << name[i] << " upper not plus infinity" << endl;
					ok = false;
				}
				if( ! ok )
				{	msg << "child_smooth_id= " << child_smooth_id << endl;
					msg << "smooth_grid_id = " << grid_id         << endl;
					msg << "prior_id       = " << prior_id[i]     << endl;
					std::string table_name  = "rate";
					table_error_exit( table_name, rate_id, msg.str() );
				}
			}
		}
	}
}

} // END DISMOD_AT_NAMESPACE
