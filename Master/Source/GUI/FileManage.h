#ifndef __FILEMANAGE_H
#define __FILEMANAGE_H

#include "SdFat.h"

void TextEditorSet(char* text, SdFile file);
bool TextEditorGetLocked();

#endif
