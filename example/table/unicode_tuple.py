# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin unicode_tuple.py}
# {xrst_spell
#     tuple
# }
# {xrst_comment_ch #}
#
# unicode_tuple: Example and Test
# ###############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end unicode_tuple.py}
# BEGIN PYTHON
#
def unicode_tuple() :
   import dismod_at
   #
   iterable     = [ 1, 2.00, 'three' ]
   quote_string = True
   u_tuple      = dismod_at.unicode_tuple(iterable, quote_string)
   check        = u'( 1, 2.0, \'three\' )'
   assert u_tuple == check
   #
   print('unicode_tuple: OK')
# END PYTHON
