/*To compile without using stdafx.h
1) right click on the project SPDepo
2) select properties
3) select configuration properties
4) select C/C++
5) select Precompile Header
6) select No precompile Header

To compile with stdafx.h (a lot faster)
Forced included of stdafx.h is needed in everyfile
1) right click on the project SPDepo
2) select properties
3) select configuration properties
4) select C/C++
5) select Advanced
6) select Force Included Files
7) add stdafx.h into the force included files section
Make sure that precompile header option is used
(check precompile header tab)
*/


// stdafx.cpp : source file that includes just the standard includes
// SPDepo.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
