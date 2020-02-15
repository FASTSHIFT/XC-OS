#ifndef __USB_PRIVATE_H
#define __USB_PRIVATE_H

#include "Basic/FileGroup.h"
#include "Shell/Shell.h"

/*Shell*/
void Shell_Init();

void ShellReg_HW();

/*FileSystem*/
const char* FS_GetPromptMsg();
void ShellReg_FS();

#endif
