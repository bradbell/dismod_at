// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin weight_info$$
$spell
	const
	CppAD
	struct
	w_info
$$

$section Extract and Organize Information for One Weighting Function$$

$head Syntax$$
$codei%weight_info %w_info%(%weight_id% , %weight_grid_table%)
%$$
$codei%weight_info %w_test%( %age_id%, %time_id%, %weight%)
%$$
$codei%weight_info %w_default%()
%$$
$icode%w_default% = %w_info%
%$$
$icode%n_age%     = %w_info%.age_size()
%$$
$icode%n_time%    = %w_info%.time_size()
%$$
$icode%a_id%      = %w_info%.age_id(%i%)
%$$
$icode%t_id%      = %w_info%.time_id(%j%)
%$$
$icode%w%         = %w_info%.weight(%i%, %j%)
%$$

$head Purpose$$
Extracts the information for one weighting from
the $cref weight_grid_table$$.
In addition, this routine checks the $code weight_info$$ table
$cref/rectangular grid/weight_grid_table/Rectangular Grid/$$ assumption.

$head w_info$$
In all its uses, except during construction,
this object has prototype
$codei%
	const weight_info %w_info%
%$$
The mean of the corresponding constructor arguments are specified below:

$subhead weight_id$$
This argument has prototype
$codei%
	size_t %weight_id%
%$$
and is the $cref/weight_id/weight_grid_table/weight_id/$$ for the
weighting that $icode w_info$$ corresponds to.

$subhead weight_grid_table$$
This argument has prototype
$codei%
	const CppAD::vector<weight_grid_struct>& %weight_grid_table%
%$$
an is the $cref weight_grid_table$$.

$subhead w_info$$
This result has type $code weight_info$$.
All of the functions calls in the syntax above are $code const$$; i.e.,
they do not modify $icode w_info$$.

$head w_test$$
This constructor is used for testing purposes only.
The meaning of its arguments are specified below:

$subhead age_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %age_id%
%$$
It specifies the age grid indices; i.e.
$codei%
	%w_info%.age_id(%i%) = %age_id%[%i%]
%$$

$subhead time_id$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %time_id%
%$$
It specifies the time grid indices; i.e.
$codei%
	%w_info%.time_id(%i%) = %time_id%[%i%]
%$$

$subhead weight$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %weight%
%$$
It specifies the weight grid values in row major order; i.e.
$codei%
	%w_info%.weight(%i%, %j%) = %time_id%[%i%*%n_time% + %j%]
%$$

$head w_default$$
This is the default constructor. It can be used to create
an empty $code weight_info$$ object that is later set equal
to another $code weight_info$$ object.
This is useful when creating vectors of such objects.

$head n_age$$
This result has prototype
$codei%
	size_t %n_age%
%$$
and is the number of age values for this weighting.

$head n_time$$
This result has prototype
$codei%
	size_t %n_time%
%$$
and is the number of time values for this weighting.

$head i$$
The argument $icode i$$ has prototype
$codei%
	size_t %i%
%$$
and is the age index; $icode%i% < %n_age%$$.

$head j$$
The argument $icode j$$ has prototype
$codei%
	size_t %j%
%$$
and is the time index; $icode%j% < %n_time%$$.

$head a_id$$
This return value has prototype
$codei%
	size_t %a_id%
%$$
and is the $th i$$ $cref/age_id/weight_grid_table/age_id/$$
for this weighting and increases with $icode i$$; i.e.,
for $icode%i% < %n_age%-2%$$
$codei%
	%w_info%.age_id(%i%) < %w_info%.age_id(%i%+1)
%$$

$head t_id$$
This return value has prototype
$codei%
	size_t %t_id%
%$$
and is the $th j$$ $cref/time_id/weight_grid_table/time_id/$$
for this weighting and increases with $icode j$$; i.e.,
for $icode%j% < %n_time%-2%$$
$codei%
	%w_info%.time_id(%j%) < %w_info%.time_id(%j%+1)
%$$

$head w$$
This return value has prototype
$codei%
	double %w%
%$$
and is the weighting for the corresponding age and time indices.

$children%example/devel/table/weight_info_xam.cpp
%$$
$head Example$$
The file $cref weight_info_xam.cpp$$ contains an example that uses
this function.

$end
*/
# include <dismod_at/weight_info.hpp>

namespace {
	void unique_insert_sort(
		CppAD::vector<size_t>& vec     ,
		size_t                 element )
	{	size_t n = vec.size();
		size_t i = 0;
		while( i < n && vec[i] < element )
			i++;
		if( i == n )
		{	vec.push_back(element);
			return;
		}
		if( vec[i] == element )
			return;
		vec.push_back( vec[n-1] );
		size_t j = n - 1;
		while( j > i )
			vec[j] = vec[j-1];
		vec[i] = element;
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

size_t weight_info::age_size(void) const
{	return age_id_.size(); }
size_t weight_info::time_size(void) const
{	return time_id_.size(); }
//
size_t weight_info::age_id(size_t i) const
{	return age_id_[i]; }

size_t weight_info::time_id(size_t j) const
{	return time_id_[j]; }
//
double weight_info::weight(size_t i, size_t j) const
{	assert( i < age_id_.size() );
	assert( j < time_id_.size() );
	return weight_[ i * time_id_.size() + j];
}

// Assignment operator
void weight_info::operator=(const weight_info& w_info)
{	age_id_ =  w_info.age_id_;
	time_id_ = w_info.time_id_;
	weight_  = w_info.weight_;
}

// Default constructor
weight_info::weight_info(void)
:
age_id_(0),
time_id_(0) ,
weight_(0)
{ }

// Testing Constructor
weight_info::weight_info(
	const CppAD::vector<size_t>& age_id    ,
	const CppAD::vector<size_t>& time_id   ,
	const CppAD::vector<double>& weight    )
{	age_id_  = age_id;
	time_id_ = time_id;
	weight_  = weight;
}
// Normal Constructor
weight_info::weight_info(
	size_t                                   weight_id         ,
	const CppAD::vector<weight_grid_struct>& weight_grid_table )
{	size_t i, j, id;

	using std::cerr;
	using std::endl;
	using std::string;

	// determine the vector age_id and time_id vectors for this weight_id
	assert( age_id_.size() == 0 );
	assert( time_id_.size() == 0 );
	size_t n_weight = weight_grid_table.size();
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == int(weight_id) )
		{	id  = weight_grid_table[i].age_id;
			unique_insert_sort( age_id_,  id );
			id  = weight_grid_table[i].time_id;
			unique_insert_sort( time_id_, id );
		}
	}

	// number of age and time points for this weighting
	size_t n_age  = age_id_.size();
	size_t n_time = time_id_.size();

	// set weight_ and count number of times each
	// age, time pair appears for this weight_id
	CppAD::vector<size_t> count(n_age * n_time );
	weight_.resize(n_age  * n_time );
	for(i = 0; i < n_age * n_time; i++)
		count[i] = 0;
	for( i = 0; i < n_weight; i++)
	{	if( weight_grid_table[i].weight_id == int(weight_id) )
		{	id           = weight_grid_table[i].age_id;
			size_t j_age = n_age;
			for(j = 0; j < n_age; j++ )
				if( id == age_id_[j] )
					j_age = j;
			assert( j_age < n_age );
			//
			id            = weight_grid_table[i].time_id;
			size_t j_time = n_time;
			for(j = 0; j < n_time; j++ )
				if( id == time_id_[j] )
					j_time = j;
			assert( j_time < n_time );
			size_t index = j_age * n_time + j_time;
			count[index]++;
			//
			weight_[index] = weight_grid_table[i].weight;
		}
	}

	// make sure each age, time pair appears once
	for(i = 0; i < n_age * n_time; i++)
	{	if( count[i] != 1 )
		{	size_t j_time = i % n_time;
			size_t j_age  = (i - j_time) / n_time;
			cerr << "weight_grid table with weight_id = " << weight_id
			<< endl << "age_id = " << age_id_[j_age]
			<< ", time_id = " << time_id_[j_time] << " appears "
			<< count[i] << " times (not 1 time)." << endl;
		}
	}
}


} // END_DISMOD_AT_NAMESPACE
