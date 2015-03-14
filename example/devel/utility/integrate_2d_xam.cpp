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
$begin integrate_2d_xam.cpp$$
$spell
	xam
$$

$section C++ integrate_1d: Example and Test$$
$index example, C++ integrate_1d$$
$index integrate_1d, C++ example$$

$code
$verbatim%example/devel/utility/integrate_2d_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <utility>
# include <limits>
# include <cmath>
# include <dismod_at/integrate_2d.hpp>

namespace {
	double U(double a, double t, const CppAD::vector<double>& u)
	{	return  u[0] + u[1] * a + u[2] * t + u[3] * a * t; }
	double integral(
		const std::pair<double,double>&    b,
		const std::pair<double,double>&    s,
		const CppAD::vector<double>&       w,
		const CppAD::vector<double>&       u)
	{
		double b1 = b.first;
		double b2 = b.second;
		double s1 = s.first;
		double s2 = s.second;
		//
		double w11 =  w[0];
		double w12 =  w[1];
		double w21 =  w[2];
		double w22 =  w[3];
		//
		double u11 =  U(b1, s1, u);
		double u12 =  U(b1, s2, u);
		double u21 =  U(b2, s1, u);
		double u22 =  U(b2, s2, u);
		//
		double v11 = w11 * u11;
		double v12 = w12 * u12;
		double v21 = w21 * u21;
		double v22 = w22 * u22;
		//
		double r = (b2 - b1) * (s2 - s1) / 4.0;
		//
		return r * (v11 + v12 + v21 + v22);
	}
}
bool integrate_2d_xam(void)
{
	bool   ok = true;
	double eps = std::numeric_limits<double>::epsilon();

	CppAD::vector<double> w(4);
	w[0] = 0.7;
	w[1] = 0.9;
	w[2] = 1.1;
	w[3] = 1.3;

	std::pair<double,double> a, t, b, s;
	//
	a.first  =  0.1;
	a.second =  1.1;
	b.first  =  0.2;
	b.second =  0.9;
	//
	t.first  =  1.1;
	t.second =  2.1;
	s.first  =  1.3;
	s.second =  1.9;
	//
	CppAD::vector<double> c;
	c = dismod_at::integrate_2d(a, t, b, s, w);
	//
	CppAD::vector<double> u(4);
	u[0] = 1.5;
	u[1] = 1.7;
	u[2] = 1.9;
	u[3] = 2.1;
	//
	double u11 =  U(a.first,  t.first, u);
	double u12 =  U(a.first,  t.second, u);
	double u21 =  U(a.second, t.first, u);
	double u22 =  U(a.second, t.second, u);
	//
	double I     = c[0] * u11 + c[1] * u12 + c[2] * u21 + c[3] * u22;
	double check = integral(b, s, w, u);
	//
	ok = std::fabs( 1.0 - I / check ) < eps * 100;
	return ok;
}
// END C++
