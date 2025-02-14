#ifndef S3TC_H
#define S3TC_H
#include "common/scummsys.h"

unsigned long PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void DecompressBlockDXT1(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, uint32 *image);
void BlockDecompressImageDXT1(unsigned long width, unsigned long height, const unsigned char *blockStorage, uint32 *image);
void DecompressBlockDXT5(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, uint32 *image);
void BlockDecompressImageDXT5(unsigned long width, unsigned long height, const unsigned char *blockStorage, uint32 *image);

#endif // S3TC_H
