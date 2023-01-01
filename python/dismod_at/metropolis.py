# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin metropolis$$ $newlinech #$$
# $spell
#  str
#  sqlite
#  dismod
#  numpy
#  inf
# $$
#
# $nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Metropolis MCMC Algorithm$$
#
# $head Syntax$$
# $codei%(%a%, %c%) = dismod_at.metropolis(%log_f%, %m%, %x0%, %s%)
# %$$
#
# $head log_f$$
# Given a numpy $code float$$ vector of length $icode n$$, the syntax
# $codei%
#  %d% = %log_f%(%x%)
# %$$
# sets the $code float$$ $icode d$$ to the log of the un-normalized density
# corresponding to the positive function $latex f(x)$$ mapping
# $latex \B{R}^n$$ to the non-negative real values.
# If $latex f(x)$$ is zero, the corresponding log-density value should equal
# $code - float("inf")$$.
#
# $head m$$
# is the $code int$$ number of vectors in the MCMC chain.
#
# $head x0$$
# is a numpy $code float$$ vector of length $icode n$$
# that specifies the initial vector in the chain
# (denoted by $latex x^0$$ below).
#
# $head s$$
# is a $code float$$ or, a numpy $code float$$ vector of length $icode n$$,
# that specifies the scaling for each of the components of $latex x$$.
#
# $subhead Vector Case$$
# If $icode s$$ is a vector,
# for $latex i = 1 , \ldots, m-1$$,
# and $latex j = 0 , \ldots, n-1$$,
# the $th j$$ component of the $th i$$ proposal vector $latex y^i$$
# is given by
# $latex \[
#  y_j^i = x_j^{i-1} + w_j^{i-1} s_j
# \] $$
# where $latex w_j^i \sim \B{N}(0, 1)$$ are all independent.
#
# $subhead Float Case$$
# If $icode s$$ is a $code float$$,
# $latex \[
#  y_j^i = x_j^{i-1} + w_j^{i-1} s
# \] $$
#
# $head a$$
# is the $code int$$ acceptance count; i.e. the number of indices $latex i$$
# such that $latex x^i = y^i$$
# (for the other indices $latex x^i = x^{i-1}$$).
#
# $head c$$
# is an $latex m \times n$$ numpy $code float$$ array that contains
# the components of the Markov Chain.
# We use the notation $latex x_j^i$$ for $icode%c%[%i%, %j%]%$$.
# For any smooth function $latex g : \B{R}^n \rightarrow \B{R}$$,
# the Metropolis algorithm provides the following approximation as
# $latex m \rightarrow \infty$$,
# $latex \[
#  \frac{1}{m} \sum_{i=0}^{m-1} g( x^i )
#  \rightarrow
#  \frac{ \int g( x ) f ( x ) \B{d} x  }{ \int f( x ) \B{d} x }
# \] $$
#
# $children%example/user/metropolis.py
# %$$
# $head Example$$
# The file $cref user_metropolis.py$$ contains an example and test of this
# routine.
#
# $end
# ---------------------------------------------------------------------------
def metropolis(log_f, m, x0, s) :
   import math
   import numpy
   import scipy.stats
   if not isinstance(s, float) :
      assert len(s) == len(x0)
   inf    = float("inf")
   n      = len(x0)
   x      = x0
   lpx    = log_f(x0)
   assert lpx > -inf
   c      = numpy.zeros((m, n), dtype=float)
   c[0:]  = x
   a      = 0
   for i in range(m-1) :
      w     = scipy.stats.norm.rvs(loc = 0.0, scale = 1.0, size=n)
      y     = x + s * w
      lpy   = log_f(y)
      if lpy > -inf :
         alpha = math.exp( lpy - lpx )
         u     = scipy.stats.uniform.rvs(loc = 0.0, scale = 1.0, size=1)
         if alpha >= u :
            a   = a + 1
            x   = y
            lpx = lpy
      c[i+1:] = x
   return (a, c)
