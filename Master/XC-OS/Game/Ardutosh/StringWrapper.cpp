#include "StringWrapper.h"
#include "Platform.h"
#include "Defines.h"
#include <string.h>

int xString::CalculateLength() const 
{
	if (!data)
		return 0;
	if (type == Type::Flash)
	{
		return strlen_P(data);
	}
	return strlen(data);
}

char xString::operator [] (int i) const
{
	if (i >= 0 && i < Length())
	{
		return Read(i);
	}
	return '\0';
}

char xString::Read(int i) const
{
	if (type == Type::Flash)
	{
		return pgm_read_byte(&data[i]);
	}
	return data[i];
}

int xString::GetIndex(char search, int start) const
{
	int length = Length();
	if (start < length)
	{
		for (int i = start; i < length; i++)
		{
			if (Read(i) == search)
			{
				return i;
			}
		}
	}
	return -1;
}

int xString::GetWordLength(int index)  const
{
	int length = Length();
	int count = 0;

	while(index < length)
	{
		char c = Read(index);
		if (c == ' ' || c == '\n' || c == '\r')
		{
			break;
		}
		index++;
		count++;
	}
	return count;
}

int xString::NumLines(int maxCharactersPerLine) const
{
	int length = Length();
	int count = 0;
	int index = 0;

	while (index < length)
	{
		count++;
		index = GetLineEndIndex(index, maxCharactersPerLine);

		// If we end with a new line then count this as an extra line
		if (index == length && (Read(index - 1) == '\n' || Read(index - 1) == '\r'))
		{
			count++;
		}
	}

	return count;
}

int xString::GetLineStartIndex(int lineNumber, int maxCharactersPerLine) const
{
	int length = Length();
	int index = 0;

	while (index < length)
	{
		if (lineNumber == 0)
		{
			break;
		}
		index = GetLineEndIndex(index, maxCharactersPerLine);
		lineNumber--;
	}

	return index; 
}

int xString::GetLineEndIndex(int start, int maxCharactersPerLine) const
{
	int length = Length();
	int index = start;

	while (index < length)
	{
		char c = Read(index);
		if (c == '\n' || c == '\r')
		{
			return index + 1;
		}

		if (index - start >= maxCharactersPerLine)
			return index;

		if (c != ' ')
		{
			int wordLength = GetWordLength(index);
			if (index + wordLength - start < maxCharactersPerLine)
			{
				index += wordLength;
			}
			else if (index == start)
			{
				return start + maxCharactersPerLine - 1;
			}
			else
			{
				return index;
			}
		}
		else
		{
			index++;
		}
	}

	return length;
}

void xString::Insert(char c, int index)
{
	int length = Length();

	for (int n = length; n > index; n--)
	{
		data[n] = data[n - 1];
	}
	data[index] = c;

	if (index == length)
	{
		data[index + 1] = '\0';
	}
}

void xString::Remove(int index)
{
	int length = Length();

	for (int n = index; n < length; n++)
	{
		data[n] = data[n + 1];
	}
}
