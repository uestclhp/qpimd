/* BGP4 SNMP support
   Copyright (C) 1999, 2000 Kunihiro Ishiguro

This file is part of GNU Kroute.

GNU Kroute is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Kroute is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Kroute; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _BANE_BGP_SNMP_H
#define _BANE_BGP_SNMP_H

extern void bgp_snmp_init (void);
extern void bgpTrapEstablished (struct peer *);
extern void bgpTrapBackwardTransition (struct peer *);

#endif /* _BANE_BGP_SNMP_H */
