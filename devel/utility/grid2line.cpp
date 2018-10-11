/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin grid2line$$
$spell
$$

$section Interpolation from Smoothing Grid to a Line$$

$head Syntax$$
$icode%line_value% = grid2line(
%line_age%, %line_time%, %age_table%, %time_table%, %s_info%, %smooth_value%
)%$$

$head Prototype$$
$srcfile%devel/utility/grid2line.cpp%
	0%// BEGIN PROTOTYPE%// END PROTOTYPE%1
%$$

$head n_line$$
We use $icode n_line$$ to denote the number of
points in the line.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head line_age$$
This vector has size $icode n_line$$ and contains the age value
corresponding to each of the points in the line.
It must be monotone non-decreasing; i.e.,
$codei%
	%line_age%[%k%] <= %line_age%[%k%+1]
%$$

$head line_time$$
This vector has size $icode n_line$$ and contains the time value
corresponding to each of the points in the line.
It must be monotone non-decreasing; i.e.,
$codei%
	%line_time%[%k%] <= %line_time%[%k%+1]
%$$

$head age_table$$
This argument is the $cref age_table$$.

$head time_table$$
This argument is the $cref time_table$$.

$head s_info$$
This is the $cref/smoothing information/smooth_info/$$ for the value
being interpolated.

$head smooth_value$$
Is the values corresponding to the smoothing grid points.
Let $icode n_age$$ and $icode n_time$$ be the number of age and
time points in the smoothing grid.
For $icode%i% = 0 , %...%. %n_age%-1%$$,
For $icode%j% = 0 , %...%. %n_time%-1%$$,
$codei%
	%smooth_value%[ %i% * %n_time% + %j% ]
%$$
is the value corresponding to the $th i$$ age and $th j$$ time
in the smoothing grid.

$head line_value$$
The return value $icode line_value$$ has size $icode n_line$$.
For each $icode i$$,
$icode%line_value%[%i%]%$$ is the
$cref bilinear$$ interpolated value corresponding to
age $icode%line_age%[%i%]%$$ and time $icode%line_time%[%i%]%$$.

$children%example/devel/utility/grid2line_xam.cpp
%$$
$head Example$$
The file $cref grid2line_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
# include <dismod_at/grid2line.hpp>
# include <dismod_at/a1_double.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN PROTOTYPE
template <class Float>
CppAD::vector<Float> grid2line(
	const CppAD::vector<double>& line_age     ,
	const CppAD::vector<double>& line_time    ,
	const CppAD::vector<double>& age_table    ,
	const CppAD::vector<double>& time_table   ,
	const smooth_info&           s_info       ,
	const CppAD::vector<Float>&  smooth_value )
// END PROTOTYPE
{	//
	assert( line_age.size() == line_time.size() );
	//
	size_t n_line = line_age.size();
	CppAD::vector<Float> line_value(n_line);
	//
	// number of age and time points in the grid
	size_t n_age  = s_info.age_size();
	size_t n_time = s_info.time_size();
	//
	double age_min  = age_table[  s_info.age_id(0) ];
	double time_min = time_table[ s_info.time_id(0) ];
	//
	double age_max  = age_table[  s_info.age_id(n_age - 1) ];
	double time_max = time_table[ s_info.time_id(n_time - 1) ];
	//
	size_t i = 1;
	size_t j = 1;
	for(size_t k = 0; k < n_line; ++k)
	{	double age      = line_age[k];
		double time     = line_time[k];
		//
		// determine interval for this age
		bool one_age;
		if( age <= age_min )
		{	one_age = true;
			i       = 0;
		}
		else if( age_max <= age )
		{	one_age = true;
			i       = n_age - 1;
		}
		else
		{	one_age = false;
			while( i < n_age - 1 && age_table[ s_info.age_id(i) ] < age )
				++i;
		}
		//
		// determine interval for this time
		bool one_time;
		if( time <= time_min )
		{	one_time = true;
			j        = 0;
		}
		else if( time_max <= time )
		{	one_time = true;
			j        = n_time - 1;
		}
		else
		{	one_time = false;
			while( j < n_time - 1 && time_table[ s_info.time_id(j) ] < time )
				++j;
		}
		// index in smooth_value corresponding to grid point (i, j)
		size_t ij_smooth = i * n_time + j;
		//
		// result of the interpolation for this point on the line
		Float res = Float(0);
		//
		// case with no interpolation
		if( one_age & one_time )
			res =  smooth_value[ij_smooth];
		//
		if( one_time )
		{	// case with only age interpolation
			assert( i > 0 );
			Float  vp = smooth_value[ij_smooth];
			double ap = age_table[ s_info.age_id(i) ];
			Float  vm = smooth_value[ij_smooth - n_time];
			double am = age_table[ s_info.age_id(i - 1) ];
			assert( am <= age && age <= ap );
			res       = vm * (ap - age);
			res      += vp * (age - am);
			res      /= (ap - am);
		}
		else if( one_age )
		{	// case with only time interpolation
			assert( j > 0 );
			Float  vp = smooth_value[ij_smooth];
			double tp = time_table[ s_info.time_id(j) ];
			Float  vm = smooth_value[ij_smooth - 1];
			double tm = time_table[ s_info.time_id(j - 1) ];
			assert( tm <= time && time <= tp );
			res       = vm * (tp - time);
			res      += vp * (time - tm);
			res      /= (tp - tm);
		}
		else
		{	// interpolate in both age and time
			assert( i > 0 );
			assert( j > 0 );
			double ap  = age_table[ s_info.age_id(i) ];
			double am  = age_table[ s_info.age_id(i - 1) ];
			double tp  = time_table[ s_info.time_id(j) ];
			double tm  = time_table[ s_info.time_id(j - 1) ];
			Float  vpp = smooth_value[ij_smooth];
			Float  vmp = smooth_value[ij_smooth - n_time];
			Float  vpm = smooth_value[ij_smooth - 1];
			Float  vmm = smooth_value[ij_smooth - n_time - 1];
			assert( am <= age && age <= ap );
			assert( tm <= time && time <= tp );
			res        = vpp * (age - am) * (time - tm);
			res       += vmm * (ap - age) * (tp - time);
			res       += vmp * (ap - age) * (time - tm);
			res       += vpm * (age - am) * (tp - time);
			res       /=(ap - am) * (tp - tm);
		}
		line_value[k] = res;
	}
	return line_value;
}


// instantiation
# define DISMOD_AT_INSTANTIATE_GRID2LINE(Float)  \
template CppAD::vector<Float> grid2line(         \
	const CppAD::vector<double>& line_age     ,    \
	const CppAD::vector<double>& line_time    ,    \
	const CppAD::vector<double>& age_table    ,    \
	const CppAD::vector<double>& time_table   ,    \
	const smooth_info&           s_info       ,    \
	const CppAD::vector<Float>&  smooth_value      \
);

DISMOD_AT_INSTANTIATE_GRID2LINE( double )
DISMOD_AT_INSTANTIATE_GRID2LINE( a1_double )

} // END DISMOD_AT_NAMESPACE
