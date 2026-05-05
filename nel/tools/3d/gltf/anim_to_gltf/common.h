#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <nel/misc/cmd_args.h>


std::string getLongArgFirstValue(const NLMISC::CCmdArgs &args, const std::string &argName);

#endif // COMMON_H
