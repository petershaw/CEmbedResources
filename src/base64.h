/* 
 * File:   base64.h
 * Author: pshaw
 *
 * Created on April 2, 2012, 10:21 AM
 */

#ifndef BASE64_H
#define	BASE64_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif

int   decode_base64(unsigned char *dest, const char *src);
char *encode_base64(int size, unsigned char *src);

#ifdef	__cplusplus
}
#endif

#endif	/* BASE64_H */

