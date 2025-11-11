#ifndef DOS_PLATFORM_MDA_DEBUG_OUTPUT_CPP
#define DOS_PLATFORM_MDA_DEBUG_OUTPUT_CPP

void putMdaChar(int col, int row, char character, char attribute);
void clearMdaScreen();
void mdaPrint(const char* message, int x, int y, char attribute);

#endif // DOS_PLATFORM_MDA_DEBUG_OUTPUT_CPP