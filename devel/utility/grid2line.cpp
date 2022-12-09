// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin grid2line}
{xrst_spell
   interpolated
}

Interpolation from Smoothing Grid to a Line
###########################################

Syntax
******

| *line_value* = ``grid2line`` (
| *line_age* , *line_time* , *age_table* , *time_table* , *g_info* , *grid_value*
| )

Prototype
*********
{xrst_literal
   // BEGIN PROTOTYPE
   // END PROTOTYPE
}

n_line
******
We use *n_line* to denote the number of
points in the line.

Grid_info
*********
The type *Grid* must be :ref:`smooth_info-name`
or :ref:`weight_info-name` .

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

line_age
********
This vector has size *n_line* and contains the age value
corresponding to each of the points in the line.
It is faster if successive points have close values in age; e.g.,

   *line_age* [ *k* ] <= *line_age* [ *k* +1]

line_time
*********
This vector has size *n_line* and contains the time value
corresponding to each of the points in the line.
It is faster if successive points have close values in time; e.g.,

   *line_time* [ *k* ] <= *line_time* [ *k* +1]

age_table
*********
This argument is the :ref:`age_table-name` .

time_table
**********
This argument is the :ref:`time_table-name` .

g_info
******
This is the information for the grid that is being interpolated.

grid_value
**********
Is the values corresponding to each of the grid points
in *g_info* .
Let *n_age* and *n_time* be the number of age and
time points in the grid.
For *i* = 0 , .... *n_age* ``-1`` ,
For *j* = 0 , .... *n_time* ``-1`` ,

   *grid_value* [ *i* * *n_time* + *j*  ]

is the value corresponding to the *i*-th age and *j*-th time
in the grid.

line_value
**********
The return value *line_value* has size *n_line* .
For each *i* ,
*line_value* [ *i* ] is the
:ref:`bilinear-name` interpolated value corresponding to
age *line_age* [ *i* ] and time *line_time* [ *i* ] .
{xrst_toc_hidden
   example/devel/utility/grid2line_xam.cpp
}
Example
*******
The file :ref:`grid2line_xam.cpp-name` contains an example and test
of using this routine.

{xrst_end grid2line}
*/
# include <dismod_at/grid2line.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/weight_info.hpp>


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN PROTOTYPE
template <class Grid_info, class Float>
CppAD::vector<Float> grid2line(
   const CppAD::vector<double>& line_age     ,
   const CppAD::vector<double>& line_time    ,
   const CppAD::vector<double>& age_table    ,
   const CppAD::vector<double>& time_table   ,
   const Grid_info&             g_info       ,
   const CppAD::vector<Float>&  grid_value )
// END PROTOTYPE
{  //
   assert( line_age.size() == line_time.size() );
   //
   size_t n_line = line_age.size();
   CppAD::vector<Float> line_value(n_line);
   //
   // number of age and time points in the grid
   size_t n_age  = g_info.age_size();
   size_t n_time = g_info.time_size();
   //
   double age_min  = age_table[  g_info.age_id(0) ];
   double time_min = time_table[ g_info.time_id(0) ];
   //
   double age_max  = age_table[  g_info.age_id(n_age - 1) ];
   double time_max = time_table[ g_info.time_id(n_time - 1) ];
   //
   size_t i = 1;
   size_t j = 1;
   for(size_t k = 0; k < n_line; ++k)
   {  double age      = line_age[k];
      double time     = line_time[k];
      //
      // determine interval for this age
      bool one_age;
      if( age <= age_min )
      {  one_age = true;
         i       = 0;
      }
      else if( age_max <= age )
      {  one_age = true;
         i       = n_age - 1;
      }
      else
      {  one_age = false;
         while( i < n_age - 1 && age_table[ g_info.age_id(i) ] < age )
            ++i;
         while( 1 < i && age < age_table[ g_info.age_id(i-1) ] )
            --i;
      }
      //
      // determine interval for this time
      bool one_time;
      if( time <= time_min )
      {  one_time = true;
         j        = 0;
      }
      else if( time_max <= time )
      {  one_time = true;
         j        = n_time - 1;
      }
      else
      {  one_time = false;
         while( j < n_time - 1 && time_table[ g_info.time_id(j) ] < time )
            ++j;
         while( 1 < j && time < time_table[ g_info.time_id(j-1) ] )
            --j;
      }
      // index in grid_value corresponding to grid point (i, j)
      size_t ij_smooth = i * n_time + j;
      //
      // result of the interpolation for this point on the line
      Float res = Float(0);
      //
      // case with no interpolation
      if( one_age & one_time )
         res =  grid_value[ij_smooth];
      else if( one_time )
      {  // case with only age interpolation
         assert( i > 0 );
         Float  vp = grid_value[ij_smooth];
         double ap = age_table[ g_info.age_id(i) ];
         Float  vm = grid_value[ij_smooth - n_time];
         double am = age_table[ g_info.age_id(i - 1) ];
         assert( am <= age && age <= ap );
         res       = vm * (ap - age);
         res      += vp * (age - am);
         res      /= (ap - am);
      }
      else if( one_age )
      {  // case with only time interpolation
         assert( j > 0 );
         Float  vp = grid_value[ij_smooth];
         double tp = time_table[ g_info.time_id(j) ];
         Float  vm = grid_value[ij_smooth - 1];
         double tm = time_table[ g_info.time_id(j - 1) ];
         assert( tm <= time && time <= tp );
         res       = vm * (tp - time);
         res      += vp * (time - tm);
         res      /= (tp - tm);
      }
      else
      {  // interpolate in both age and time
         assert( i > 0 );
         assert( j > 0 );
         double ap  = age_table[ g_info.age_id(i) ];
         double am  = age_table[ g_info.age_id(i - 1) ];
         double tp  = time_table[ g_info.time_id(j) ];
         double tm  = time_table[ g_info.time_id(j - 1) ];
         Float  vpp = grid_value[ij_smooth];
         Float  vmp = grid_value[ij_smooth - n_time];
         Float  vpm = grid_value[ij_smooth - 1];
         Float  vmm = grid_value[ij_smooth - n_time - 1];
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
# define DISMOD_AT_INSTANTIATE_GRID2LINE(Grid_info, Float)  \
template CppAD::vector<Float> grid2line(                    \
   const CppAD::vector<double>& line_age     ,             \
   const CppAD::vector<double>& line_time    ,             \
   const CppAD::vector<double>& age_table    ,             \
   const CppAD::vector<double>& time_table   ,             \
   const Grid_info&             g_info       ,             \
   const CppAD::vector<Float>&  grid_value                 \
);

DISMOD_AT_INSTANTIATE_GRID2LINE( weight_info, double )
DISMOD_AT_INSTANTIATE_GRID2LINE( smooth_info, double )
//
DISMOD_AT_INSTANTIATE_GRID2LINE( weight_info, a1_double )
DISMOD_AT_INSTANTIATE_GRID2LINE( smooth_info, a1_double )

} // END DISMOD_AT_NAMESPACE
