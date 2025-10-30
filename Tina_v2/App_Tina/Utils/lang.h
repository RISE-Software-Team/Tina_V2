/*
 * lang.h
 *
 *  Created on: Oct 30, 2025
 *      Author: iancu
 */

#ifndef LANG_H_
#define LANG_H_

#define min(a,b) \
    ({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
       _a < _b ? _a : _b; })

#define max(a,b) \
    ({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
       _a > _b ? _a : _b; })

#endif /* UTILS_LANG_H_ */
