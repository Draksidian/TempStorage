#pragma once

unsigned long crc32(const unsigned char *s, unsigned int len);
int EncryptBufferXORKey(unsigned char *buf, int BufLen, const unsigned char *Key, int KeyLen);
int EncryptBufferXORKeyRotate(unsigned char *buf, int BufLen, int XORKey);