// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin integrate_1d_xam.cpp$$
$spell
	xam
$$

$section C++ integrate_1d: Example and Test$$
$index example, C++ integrate_1d$$
$index integrate_1d, C++ example$$

$code
$srcfile%example/devel/utility/integrate_1d_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <utility>
# include <limits>
# include <cmath>
# include <dismod_at/integrate_1d.hpp>

namespace {
	double U(double r, const std::pair<double,double>& u)
	{	return u.first + u.second * r; }
	double integral(
		const std::pair<double,double>& r ,
		const std::pair<double,double>& s ,
		const std::pair<double,double>& w ,
		const std::pair<double,double>& u )
	{
		double s1 = s.first;
		double s2 = s.second;
		double u1 = U(s1, u);
		double u2 = U(s2, u);
		double w1 = w.first;
		double w2 = w.second;
		double v1 = w1 * u1;
		double v2 = w2 * u2;
		return (s2 - s1) * (v1 + v2) / 2.0;
	}
}
bool integrate_1d_xam(void)
{
	bool   ok = true;
	double eps = std::numeric_limits<double>::epsilon();

	std::pair<double,double> r, s, w, c;
	//
	r.first  =  0.1;
	r.second =  1.1;
	s.first  =  0.2;
	s.second =  0.6;
	w.first  =  0.9;
	w.second =  1.2;
	//
	c = dismod_at::integrate_1d(r, s, w);
	//
	std::pair<double,double> u;
	u.first  = 1.3;
	u.second = 1.6;
	//
	double u1 = U(r.first,  u);
	double u2 = U(r.second, u);
	//
	double I  = c.first * u1 + c.second * u2;
	//
	double check = integral(r, s, w, u);
	ok = std::fabs( 1.0 - I / check ) < eps * 100;
	return ok;
}
// END C++
