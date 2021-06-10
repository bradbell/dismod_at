// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin data_subset$$
$spell
	avgint
	covariate
	subsamples
	Subsampled
	covariates
	const
	CppAD
	struct
	obj
	cov
	sim
$$

$section Create a Subsampled Version of Data Table$$

$head Syntax$$
$codei%data_subset(
	%data_table%, %data_cov_value%, %covariate_table%, %child_object%,
	%data_subset_obj%, %data_subset_cov_value%
)%$$

$head See Also$$
$cref avgint_subset$$.

$head Purpose$$
This routine subsamples the $icode data_table$$, in the following way:
$list number$$
Only rows corresponding to the
$cref/parent_node/option_table/parent_node_id/$$,
or a descendant of the parent node, are included.
$lnext
Only rows for which the covariates satisfy the
$cref/max_difference/covariate_table/max_difference/$$ criteria
are included.
$lnext
The subsampled rows are the same as the corresponding original row
except that for each covariate, its
$cref/reference/covariate_table/reference/$$ value is subtracted
from the value of the covariate in $icode data_table$$.
$lnext
All of the
$cref/child data/data_table/node_id/Child Data/$$
is checked to make sure that it does not use a
$cref/laplace/density_table/density_name/laplace/$$ or
$cref/log_laplace/density_table/density_name/log_laplace/$$ density.
$lend

$head data_table$$
This argument has prototype
$codei%
	const CppAD::vector<data_struct>& %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.

$head data_cov_value$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %data_cov_value%
%$$
and is the $cref/data_table/get_data_table/data_cov_value/$$
covariate values.

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

$head data_subset_obj$$
This argument has prototype
$codei%
	CppAD::vector<data_subset_struct>& %data_subset_obj%
%$$
Its input size is zero and upon return
its size is the number of rows in $icode data_table$$ that satisfy
the purpose above.
The structure has all the fields that are present in
$cref/data_struct/get_data_table/data_table/data_struct/$$.

$subhead n_subset$$
We use the notation $icode%n_subset% = %data_subset_obj%.size()%$$.

$subhead subset_id$$
We use the notation $icode subset_id$$ for an index between
zero and $icode%n_subset%-1%$$,

$subhead original_id$$
There an extra field in $code data_subset_struct$$ that has
name $code original_id$$, type $code int$$.
The values in this field are equal to the
$icode original_id$$ for the corresponding row of $cref data_table$$.
The value of
$codei%
	%data_subset_obj%[%subset_id%].original_id
%$$
increases with $icode subset_id$$;
i.e., for each $icode subset_id$$ less than $icode%n_subset%-2%$$,
$codei%
	%data_subset_obj%[%subset_id%].original_id <
		%data_subset_obj%[%subset_id%+1].original_id
%$$

$subhead data_sim_value$$
There an extra field in $code data_subset_struct$$ that has
name $code data_sim_value$$, type $code double$$.
All of these values are set to $code nan$$ by this routine.
These values get replaced by simulated measurement values
when we are fitting simulated data.

$head data_subset_cov_value$$
This argument has prototype
$codei%
	CppAD::vector<double>& %data_subset_cov_value%
%$$
Its input size is zero and upon return
its size is $icode%n_subset% * %n_covariate%$$.
For each $icode subset_id$$ and
$cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
%data_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%]
= %data_cov_value%[%original_id% * %n_covariate% + %covariate_id%]
  - reference(%covariate_id%)
%$$
where
$codei%
	%original_id% = %data_subset_obj%[%subset_id%].original_id
%$$
and $codei%reference(%covariate_id%)%$$ is the
$cref/reference/covariate_table/reference/$$ value for the
corresponding $icode covariate_id$$.
Note that if the
$cref/max_difference/covariate_table/max_difference/$$
value is $code null$$ in the covariate table,
or the covariate value is $code null$$ in $cref data_table$$,
$codei%
%data_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%] = 0
%$$
Also note that
$codei%
| %data_subset_cov_value%[%subset_id% * %n_covariate% + %covariate_id%] |
<= max_difference(%covariate_id%)
%$$
where $codei%max_difference(%covariate_id%)%$$ is the
maximum difference for the corresponding $icode covariate_id$$.


$childtable%example/devel/utility/data_subset_xam.cpp
%$$
$head Example$$
The file $cref data_subset_xam.cpp$$ contains
and example and test of $code data_subset$$.
It returns true for success and false for failure.

$end
*/

# include <cmath>
# include <dismod_at/data_subset.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

void data_subset(
	const std::string&                     hold_out_integrand    ,
	const CppAD::vector<integrand_struct>& integrand_table       ,
	const CppAD::vector<density_enum>&     density_table         ,
	const CppAD::vector<data_struct>&      data_table            ,
	const CppAD::vector<double>&           data_cov_value        ,
	const CppAD::vector<covariate_struct>& covariate_table       ,
	const child_info&                      child_object          ,
	CppAD::vector<data_subset_struct>&     data_subset_obj       ,
	CppAD::vector<double>&                 data_subset_cov_value )
{	assert( data_subset_obj.size() == 0 );
	assert( data_subset_cov_value.size() == 0 );
	if( data_table.size() == 0 )
		return;
	//
	size_t n_child     = child_object.child_size();
	size_t n_data    = data_table.size();
	size_t n_covariate = covariate_table.size();
	//
	size_t n_subset = 0;
	CppAD::vector<bool> ok(n_data);
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	size_t child = child_object.table_id2child(data_id);
		if( child < n_child )
		{	int density_id = data_table[data_id].density_id;
			density_enum density = density_table[density_id];
			if( density == laplace_enum || density == log_laplace_enum )
			{	std::string message =
					"child data has laplace or log_lapace density";
				std::string table_name = "data";
				error_exit(message, table_name, data_id);
			}
		}
		// check if this data is for parent or one of its descendants
		ok[data_id] = child <= n_child;
		if( ok[data_id] )
		{	for(size_t j = 0; j < n_covariate; j++)
			{	size_t index          = data_id * n_covariate + j;
				double x_j            = data_cov_value[index];
				double reference      = covariate_table[j].reference;
				double max_difference = covariate_table[j].max_difference;
				double difference     = 0.0;
				if( ! std::isnan(x_j) )
					difference = x_j - reference;
				ok[data_id]  &= std::fabs( difference ) <= max_difference;
			}
		}
		if( ok[data_id] )
			n_subset++;
	}
	//
	data_subset_obj.resize(n_subset);
	data_subset_cov_value.resize(n_subset * n_covariate);
	size_t subset_id = 0;
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	if( ok[data_id] )
		{	data_subset_struct& one_sample( data_subset_obj[subset_id] );
			//
			for(size_t j = 0; j < n_covariate; j++)
			{	size_t index          = data_id * n_covariate + j;
				double x_j            = data_cov_value[index];
				double reference      = covariate_table[j].reference;
				double difference     = 0.0;
				if( ! std::isnan(x_j) )
					difference = x_j - reference;
				index = subset_id * n_covariate + j;
				data_subset_cov_value[index] = difference;
			}
			// values in avgint_subset_struct
			one_sample.original_id  = int( data_id );
			one_sample.integrand_id = data_table[data_id].integrand_id;
			one_sample.node_id      = data_table[data_id].node_id;
			one_sample.subgroup_id  = data_table[data_id].subgroup_id;
			one_sample.weight_id    = data_table[data_id].weight_id;
			one_sample.age_lower    = data_table[data_id].age_lower;
			one_sample.age_upper    = data_table[data_id].age_upper;
			one_sample.time_lower   = data_table[data_id].time_lower;
			one_sample.time_upper   = data_table[data_id].time_upper;
			// values not in avgint_subset_struct
			one_sample.hold_out     = data_table[data_id].hold_out;
			int density_id          = data_table[data_id].density_id;
			one_sample.density      = density_table[density_id];
			one_sample.meas_value   = data_table[data_id].meas_value;
			one_sample.meas_std     = data_table[data_id].meas_std;
			one_sample.eta          = data_table[data_id].eta;
			one_sample.nu           = data_table[data_id].nu;
			// value that depends on data_sim table
			one_sample.data_sim_value =
				std::numeric_limits<double>::quiet_NaN();
			//
			// advance to next sample
			subset_id++;
		}
	}
	return;
}

} // END DISMOD_AT_NAMESPACE
