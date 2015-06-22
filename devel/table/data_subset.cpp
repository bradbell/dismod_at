// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin data_subset$$
$spell
	covariate
	subsamples
	Subsampled
	covariates
	const
	CppAD
	struct
$$

$section Create a Subsampled Version of Data Table$$

$head Syntax$$
$icode%data_sample% = data_subset(
	%data_table%, %covariate_table%, %child_object%
)%$$

$head Purpose$$
This routine subsamples the data, and
modifies the covariate values, in the following ways:
$list number$$
Only data corresponding to nodes that are descendants of the
$cref/parent_node/fit_table/parent_node_id/$$ are included.
$lnext
Only data for which the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria
are included.
$lnext
The
$cref/reference/covariate_table/reference/$$ value is subtracted
from each of the covariate values.
$lend.

$head data_table$$
This argument has prototype
$codei%
	const CppAD::vector<data_struct>& %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.

$head covariate_table$$
This argument has prototype
$codei%
	const CppAD::vector<covariate_struct>& %covariate_table%
%$$
and is the $cref/covariate_table/get_covariate_table/covariate_table/$$.

$head child_object$$
This argument has prototype
$codei%
	const child_info& %child_object%
%$$

$head data_sample$$
The return value has prototype
$codei%
	data_subset_struct %data_sample%
%$$
Its size is the number of rows in $icode data_table$$ that satisfy
the conditions above.
The structure has all the fields that are present in
$cref/data_struct/get_data_table/data_struct/$$.

$subhead data_id$$
There an extra field in $code data_struct$$ that has
name $code data_id$$, type $code int$$.
The values in this field for $icode data_sample$$ is equal to the
$icode data_id$$ for the corresponding row of $cref data_table$$.
The value of $icode%data_sample%[%i%].data_id%$$ increases with $icode i$$;
i.e., for each $icode i$$ between zero and $icode%data_sample%.size()-2%$$,
$codei%
	%data_sample%[%i%].data_id < %data_sample%[%i%+1].data_id
%$$

$subhead x$$
For each $icode i$$ between zero and $icode%data_sample%.size()-1%$$,
we use
$codei%
	row(%i%) = %data_table%[ %data_sample%[%i%].data_id ]
%$$
to denote the corresponding row of $icode data_table$$.
For each $cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
	%data_sample%[%i%].x[%covariate_id%] =
		row(%i%).x[%covariate_id%] - reference(%covariate_id%)
%$$
where $codei%reference(%covariate_id%)%$$ is the
$cref/reference/covariate_table/reference/$$ value for the
corresponding $icode covariate_id$$.
Note that if the
$cref/max_difference/covariate_table/max_difference/$$
value is $code null$$ in the covariate table,
or the covariate value is $code null$$ in the data table,
$codei%
	%data_sample%[%i%].x[%covariate_id%] = 0
%$$
Also note that
$codei%
	| %data_sample%[%i%].x[%covariate_id%] | <= max_difference(%covariate_id%)
%$$
where $codei%max_difference(%covariate_id%)%$$ is the
maximum difference for the corresponding $icode covariate_id$$.

$end
*/

# include <cmath>
# include <dismod_at/data_subset.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<data_subset_struct> data_subset(
	const CppAD::vector<data_struct>&      data_table      ,
	const CppAD::vector<covariate_struct>& covariate_table ,
	const child_info&                      child_object    )
{	CppAD::vector<data_subset_struct> data_sample;
	if( data_table.size() == 0 )
		return data_sample;
	//
	size_t n_child     = child_object.child_size();
	size_t n_data      = data_table.size();
	size_t n_covariate = data_table[0].x.size();
	//
	data_subset_struct    one_sample;
	one_sample.x.resize(n_covariate);
	//
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	size_t child = child_object.data_id2child(data_id);
		// check if this data is for parent or one of its descendents
		bool ok = child <= n_child;
		if( ok )
		{	for(size_t j = 0; j < n_covariate; j++)
			{	double x_j            = data_table[data_id].x[j];
				double reference      = covariate_table[j].reference;
				double max_difference = covariate_table[j].max_difference;
				if( std::isnan(x_j) )
					x_j = reference;
				one_sample.x[j] = x_j - reference;
				ok  &= std::fabs( one_sample.x[j] ) <= max_difference;
			}
		}
		if( ok )
		{	one_sample.data_id      = data_id;
			one_sample.integrand_id = data_table[data_id].integrand_id;
			one_sample.density_id   = data_table[data_id].density_id;
			one_sample.node_id      = data_table[data_id].node_id;
			one_sample.weight_id    = data_table[data_id].weight_id;
			one_sample.meas_value   = data_table[data_id].meas_value;
			one_sample.age_lower    = data_table[data_id].age_lower;
			one_sample.age_upper    = data_table[data_id].age_upper;
			one_sample.time_lower   = data_table[data_id].time_lower;
			one_sample.time_upper   = data_table[data_id].time_upper;
			//
			data_sample.push_back(one_sample);
		}
	}
	return data_sample;
}
} // END DISMOD_AT_NAMESPACE
