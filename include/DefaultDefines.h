/*
 * Copyright 2013-2014 Jonas Zaddach <zaddach@eurecom.fr>, EURECOM
 * Copyright 2013-2014 Lucian Cojocar <lucian.cojocar@vu.nl>, VU Amsterdam
 *
 * You can redistribute and/or modify this program under the terms of the
 * GNU General Public License version 2 or later.
 */

#ifndef _DEFAULT_DEFINES_H
#define _DEFAULT_DEFINES_H

#ifndef WATCHDOG_EXCITE
#define WATCHDOG_EXCITE do {} while(0)
#endif /* WATCHDOG_EXCITE */

#ifndef SIGNAL_DEBUG_ENTER
#define SIGNAL_DEBUG_ENTER do {} while(0)
#endif /* SIGNAL_DEBUG_ENTER */

#ifndef SIGNAL_DEBUG_EXIT
#define SIGNAL_DEBUG_EXIT do {} while(0)
#endif /* SIGNAL_DEBUG_EXIT */

#endif /* _DEFAULT_DEFINES_H */