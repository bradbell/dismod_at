# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin unicode_tuple}
# {xrst_spell
#     iterable
#     iterate
#     tuple
# }
# {xrst_comment_ch #}
#
# Convert an Iterable Object to a Unicode String
# ##############################################
#
# Syntax
# ******
# *u_tuple* = ``dismod_at.unicode_tuple`` ( *iterable* , *quote_string* )
#
# iterable
# ********
# is any object where we can iterate over its elements
# and convert each element to a unicode value.
#
# quote_string
# ************
# is either ``True`` or ``False`` .
# If it is ``True`` , each element of *iterable*
# that is an ``str`` or a ``unicode``
# is surrounded by the single quote character in *u_tuple* .
#
# None
# ****
# The value ``None`` is converted to the unicode string ``null``
# (always without quotes).
#
# Infinity
# ********
# If *u_j* is a ``float`` and equal to plus infinity,
# the corresponding tuple value has an exponent that is ten times the
# exponent for the maximum float.
# This is so that, when written by pythons ``sqlite`` package, it
# gets converted to infinity.
# Minus infinity is handled in a similar fashion.
#
# u_tuple
# *******
# is a unicode string representation of the tuple containing the elements.
# To be specific it is given by
#
#     *u_tuple* = ``u`` '( *u_1* , *u_2* , ..., *u_n*  )'
#
# where *n* is the number of elements
# and *u_1* is the *i*-th element.
# {xrst_toc_hidden
#    example/table/unicode_tuple.py
# }
# Example
# *******
# The file :ref:`unicode_tuple.py-name` is an example use of
# ``unicode_tuple`` .
#
# {xrst_end unicode_tuple}
def unicode_infinity() :
   import sys
   import math
   exponent = int( math.log( sys.float_info.max ) / math.log(10.) ) + 10
   inf      = u'10e' + str(exponent)
   return inf
def unicode_tuple(iterable, quote_string) :
   u_tuple = u'('
   count   = 0
   for element in iterable :
      if count > 0 :
         u_tuple += u','
      count += 1
      #
      if element == None :
         u_tuple += u' null'
      elif isinstance(element, float) and element == float('inf') :
         u_tuple += u' ' + unicode_infinity()
      elif isinstance(element, float) and element == - float('inf') :
         u_tuple += u' -' + unicode_infinity()
      elif quote_string and isinstance(element, str) :
         u_tuple += u' ' + u"'" + str(element) + u"'"
      else :
         u_tuple += u' ' + str(element)
   u_tuple     += u' )'
   return u_tuple
