# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin metropolis}
# {xrst_spell
#     numpy
# }
# {xrst_comment_ch #}
#
# Metropolis MCMC Algorithm
# #########################
#
# Syntax
# ******
#
#     ( *a* , *c* ) = ``dismod_at.metropolis`` ( *log_f* , *m* , *x0* , *s* )
#
# log_f
# *****
# Given a numpy ``float`` vector of length *n* , the syntax
#
#     *d* = *log_f* ( *x* )
#
# sets the ``float`` *d* to the log of the un-normalized density
# corresponding to the positive function :math:`f(x)` mapping
# :math:`\B{R}^n` to the non-negative real values.
# If :math:`f(x)` is zero, the corresponding log-density value should equal
# ``- float("inf")`` .
#
# m
# *
# is the ``int`` number of vectors in the MCMC chain.
#
# x0
# **
# is a numpy ``float`` vector of length *n*
# that specifies the initial vector in the chain
# (denoted by :math:`x^0` below).
#
# s
# *
# is a ``float`` or, a numpy ``float`` vector of length *n* ,
# that specifies the scaling for each of the components of :math:`x`.
#
# Vector Case
# ===========
# If *s* is a vector,
# for :math:`i = 1 , \ldots, m-1`,
# and :math:`j = 0 , \ldots, n-1`,
# the *j*-th component of the *i*-th proposal vector :math:`y^i`
# is given by
#
# .. math::
#
#  y_j^i = x_j^{i-1} + w_j^{i-1} s_j
#
# where :math:`w_j^i \sim \B{N}(0, 1)` are all independent.
#
# Float Case
# ==========
# If *s* is a ``float`` ,
#
# .. math::
#
#  y_j^i = x_j^{i-1} + w_j^{i-1} s
#
# a
# *
# is the ``int`` acceptance count; i.e. the number of indices :math:`i`
# such that :math:`x^i = y^i`
# (for the other indices :math:`x^i = x^{i-1}`).
#
# c
# *
# is an :math:`m \times n` numpy ``float`` array that contains
# the components of the Markov Chain.
# We use the notation :math:`x_j^i` for *c* [ *i* , *j* ] .
# For any smooth function :math:`g : \B{R}^n \rightarrow \B{R}`,
# the Metropolis algorithm provides the following approximation as
# :math:`m \rightarrow \infty`,
#
# .. math::
#
#  \frac{1}{m} \sum_{i=0}^{m-1} g( x^i )
#  \rightarrow
#  \frac{ \int g( x ) f ( x ) \B{d} x  }{ \int f( x ) \B{d} x }
#
# {xrst_toc_hidden
#    example/user/metropolis.py
# }
# Example
# *******
# The file :ref:`user_metropolis.py-name` contains an example and test of this
# routine.
#
# {xrst_end metropolis}
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
