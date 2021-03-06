To build packages for Solaris 10:

Requirements:
-------------

- Development environment including gcc (eg as shipped with Solaris 10)

- The Package tools from Solaris 10 or Solaris Nevada/Express.

- i.manifest and r.manifest scripts as supplied with Solaris Express
  in /usr/sadm/install/scripts/ or from OpenSolaris.org:

  http://cvs.opensolaris.org/source/xref/usr/src/pkgdefs/common_files/i.manifest
  http://cvs.opensolaris.org/source/xref/usr/src/pkgdefs/common_files/r.manifest
  
  i.manifest must be at least version 1.5. Place these scripts in
  this directory if you are using Solaris 10 GA (which does not ship with
  these scripts), or in the solaris/ directory in the Bane source.


Package creation instructions:
------------------------------

1. Configure and build Bane in the top level build directory as per
normal, eg:

	./configure --prefix=/usr/local/bane \
		--localstatedir=/var/run/bane
		--enable-gcc-rdynamic --enable-opaque-lsa --enable-ospf-te \
		--enable-multipath=64 --enable-user=bane \
		--enable-ospfclient=yes --enable-ospfapi=yes  \
		--enable-group=bane --enable-nssa --enable-opaque-lsa

You will need /usr/sfw/bin and /usr/ccs/bin in your path.

2. make install in the top-level build directory, it's a good idea to make
use of DESTDIR to install to an alternate root, eg:

	gmake DESTDIR=/var/tmp/qroot install

3. In this directory (solaris/), run make packages, specifying DESTDIR if
appropriate, eg:

	gmake DESTDIR=/var/tmp/qroot packages

This should result in 4 packages being created:

	bane-libs-...-$ARCH.pkg 	- BANElibs
	bane-daemons-...-$ARCH.pkg	- BANEdaemons
	bane-doc-...-$ARCH.pkg	- BANEdoc
	bane-dev-...-$ARCH.pkg	- BANEdev
	bane-smf-...-$ARCH.pkg	- BANEsmf

BANElibs and BANEdaemons are needed for daemon runtime. BANEsmf
provides the required bits for Solaris 10+ SMF support.


Install and post-install configuration notes:
---------------------------------------------

- If you specified a user/group which does not exist per default on Solaris
  (eg bane/bane) you *must* create these before installing these on a
  system. The packages do *not* create the users.

- The configuration files are not created. You must create the configuration
  file yourself, either with your complete desired configuration, or else if
  you wish to use the telnet interface for further configuration you must
  create them containing at least:

	 password whatever

  The user which bane runs as must have write permissions on this file, no
  other user should have read permissions, and you would also have to enable
  the telnet interface (see below).

- SMF notes:

  - BANEsmf installs a svc:/network/routing/bane service, with an
    instance for each daemon
  
  - The state of all instances of bane service can be inspected with:
  
  	svcs -l svc:/network/routing/bane
  
    or typically just with a shortcut of 'bane':
    
    	svcs -l bane
  
  - A specific instance of the bane service can be inspected by specifying
    the daemon name as the instance, ie bane:<daemon>:
    
    	svcs -l svc:/network/routing/bane:kroute
    	svcs -l svc:/network/routing/bane:ospfd
    	<etc>

    or typically just with the shortcut of 'bane:<daemon>' or even
    <daemon>:
    
    	svcs -l bane:kroute
    	svcs -l ospfd
    
    Eg:
    
    # # svcs -l ripd
    fmri         svc:/network/routing/bane:ripd
    name         Bane: ripd, RIPv1/2 IPv4 routing protocol daemon.
    enabled      true
    state        online
    next_state   none
    state_time   Wed Jun 15 16:21:02 2005
    logfile      /var/svc/log/network-routing-bane:ripd.log
    restarter    svc:/system/svc/restarter:default
    contract_id  93 
    dependency   require_all/restart svc:/network/routing/bane:kroute (online)
    dependency   require_all/restart file://localhost//usr/local/bane/etc/ripd.conf (online)
    dependency   require_all/none svc:/system/filesystem/usr:default (online)
    dependency   require_all/none svc:/network/loopback (online)

  - Configuration of startup options is by way of SMF properties in a
    property group named 'bane'. The defaults should automatically be
    inline with how you configured Bane in Step 1 above. 
  
  - By default the VTY interface is disabled. To change this, see below for
    how to set the 'bane/vty_port' property as appropriate for
    /each/ service. Also, the VTY is set to listen only to localhost by
    default, you may change the 'bane/vty_addr' property as appropriate
    for both of the 'bane' service and specific individual instances of
    the 'bane' service (ie bane:kroute, bane:ospfd, etc..).
    
  - Properties belonging to the 'bane' service are inherited by all
    instances. Eg:
    
    # svcprop -p bane svc:/network/routing/bane
    bane/group astring root
    bane/retain boolean false
    bane/user astring root
    bane/vty_addr astring 127.1
    bane/vty_port integer 0
    
    # svcprop -p bane svc:/network/routing/bane:ospfd
    bane/retain_routes boolean false
    bane/group astring root
    bane/retain boolean false
    bane/user astring root
    bane/vty_addr astring 127.1
    bane/vty_port integer 0
    
    All instances will inherit these properties, unless the instance itself
    overrides these defaults. This also implies one can modify properties of
    the 'bane' service and have them apply to all daemons.
    
    # svccfg -s svc:/network/routing/bane \
    	setprop bane/vty_addr = astring: ::1
    
    # svcprop -p bane svc:/network/routing/bane
    bane/group astring root
    bane/retain boolean false
    bane/user astring root
    bane/vty_port integer 0
    bane/vty_addr astring ::1
    
    # # You *must* refresh instances to have the property change
    # # take affect for the 'running snapshot' of service state.
    # svcadm refresh bane:ospfd
    
    # svcprop -p bane svc:/network/routing/bane:ospfd
    bane/retain_routes boolean false
    bane/group astring root
    bane/retain boolean false
    bane/user astring root
    bane/vty_port integer 0
    bane/vty_addr astring ::1
    
    Other daemon-specific options/properties may be available, however they
    are not yet honoured/used (eg ospfd/apiserver on svc:/network/ospf).

  - As SMF is dependency aware, restarting network/kroute will restart all the
    other daemons.
  
  - To upgrade from one set of Bane packages to a newer release, one must
    first pkgrm the installed packages. When one pkgrm's BANEsmf all
    property configuration will be lost, and any customisations will have to
    redone after installing the updated BANEsmf package.
  
- These packages are not supported by Sun Microsystems, report bugs via the
  usual Bane channels, ie Bugzilla. Improvements/contributions of course
  would be greatly appreciated.

