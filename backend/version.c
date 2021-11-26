#include "version_config.h"

const char *ppcg_version(void)
{
	return xstr(heterosa_VERSION_MAJOR) "."
		xstr(heterosa_VERSION_MINOR) "\n";
}
