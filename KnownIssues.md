# Wrong linker-name for Prolog #
_This problem is fixed now. Use ./configure command, before compiling_

If you use Ubuntu 10.10 or lower, you will most likly have swi-prolog 5.8 installed which uses different binary names than 5.10, which is installed on later versions of Ubuntu.
You have to do ONE of the following:
(Both methods were tested on a fresh installation of Ubuntu 10.04)

1. Install swi-prolog 5.10 or later
You could for e.g. download package swi-prolog-nox at
http://packages.ubuntu.com/natty/swi-prolog-nox
(do not choose an older version of ubuntu, because 5.10 is only available since natty)
You have to remove swi-prolog first, before installing the new package ;)

2. Modify ssl-refbox/ssl-refbox.pro.
Change
```
QMAKE_LINK=swipl-ld ssl_refbox_rules_prolog.pl
```
to
```
QMAKE_LINK=plld ssl_refbox_rules_prolog.pl
```
and replace all occurrences of "swipl" with "pl"

# SWI-Prolog.h: No such file or directory #
```
ssl_refbox_rules.cc:4:24: error: SWI-Prolog.h: No such file or directory
```
If you receive an error message that states, that no SWI-Prolog.h was found, you should check your pkg-config: /usr/lib/pkgconfig/swipl.pc
On Ubuntu 10.10 and below, this file is called pl.pc
It should look something like this, but check if the pathes really exist!
if the file is called pl.pc, replace all swipl with pl ;)

```
################################################################################
prefix=/usr
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${libdir}/swi-prolog/include

PLBASE=${libdir}/swi-prolog
PLARCH=i386
PLLIBS=-lgmp -lrt -lreadline -lncurses -lm -lrt -ldl 
PLLIB=-lswipl
PLCFLAGS= -pthread -fPIC
PLLDFLAGS=-rdynamic -Wl,-Bsymbolic-functions -pthread
PLSOEXT=so

Name: swipl
Description: ISO/Edinburgh-style Prolog compiler
Version: 5.10.4
Libs: -lswipl
Cflags: -I${includedir}
################################################################################
```