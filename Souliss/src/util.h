#ifndef UTIL_H
#define UTIL_H

// Host to Network for Word
#define HTONS(n) ((((U16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
#define NTOHS(x) HTONS(x)

// Host to Network for Floating Point 
#define HTONF(o, i)		*(U8*)(o+0) = *(((U8 *)(i))+1); \
						*(U8*)(o+1) = *(((U8 *)(i))+0); \
						*(U8*)(o+2) = *(((U8 *)(i))+3); \
						*(U8*)(o+3) = *(((U8 *)(i))+2)
#define NTOHF(x) HTONF(x)

// Host to Network for Long
#define HTONL(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define NTOHL(x) HTONL(x)


#endif
