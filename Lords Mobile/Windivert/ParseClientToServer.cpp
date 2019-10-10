#include <string.h>
#include <stdio.h>
#include "PacketContentGenerator.h"
#include "ParseServerToClient.h"

//0b 00 9a 08 5b 00 00 00 77 02 26
//0b 00 9a 08 64 00 00 00 67 02 f2
//0b 00 9a 08 68 00 00 00 77 02 03
//0b 00 9a 08 6b 00 00 00 77 02 12
//0b 00 9a 08 70 00 00 00 77 02 95
//0b 00 9a 08 79 00 00 00 57 02 e6
//0b 00 9a 08 c8 00 00 00 ff 03 ff
static int PacketSentCounter = 0x5B;
int OnPacketForClickCastle(unsigned char *packet, unsigned int len, int IsCastleClickPacket = 0)
{
#define CastleClickPacketBytesSize 11
	//if we receive the same packet we sent out, do not parse it again
	static char PrevPacketSent[CastleClickPacketBytesSize];
	if (memcmp(PrevPacketSent, packet, CastleClickPacketBytesSize) == 0)
		return 0;

	int x, y;
	if (GeteneratePosToScan(x, y) != 0)
		return 1; // do not change the packet
	if (IsCastleClickPacket == 0)
	{
		// set len to new size
		packet[0] = 11;
		//opcode
		packet[2] = 0x9A;
		packet[3] = 0x08;
		//unk
		packet[4] = PacketSentCounter; // query counter ? Seems to be a number that increases over time
		PacketSentCounter++;
	}
	else
	{
		PacketSentCounter = packet[4] + 1;
	}
	// write coordinates
	unsigned int GUID = GenerateIngameGUID(x, y);
	//failed to generate GUID from x, y
	if (GUID == 0)
		return 0;
	*(unsigned int*)&packet[7] = GUID;

	printf("Will try to scan map location %d %d\n", y, x);
	memcpy(PrevPacketSent, packet, CastleClickPacketBytesSize);

	return 0; // overrided content
}

int OnClientToServerPacket(unsigned char *packet, unsigned int len)
{
	//castle click packet
	//0b 00 9a 08 5b 00 00 00 77 02 26
	if (len == 11 && packet[0] == 11 && packet[1] == 0 && packet[2] == 0x9A && packet[3] == 0x08)
	{
		int ret = OnPacketForClickCastle(packet, len, 1);
		if (ret == 0)
			return 0;
	}
	//is this "delete opened gifts" packet
	//0c 00 32 0b 85 7d bd 80 35 12 75 7e
	//0c 00 32 0b 7c 9f d0 cc cc 8f e3 84
	//0c 00 32 0b 70 1c 69 60 c2 0e f0 98
	//0c 00 32 0b c5 72 33 a6 db b0 ab 86
	if (len == 12 && packet[0] == 12 && packet[1] == 0 && packet[2] == 0x32 && packet[3] == 0x0B)
	{
		int ret = OnPacketForClickCastle(packet, len);
		if (ret == 0)
			return 0;
	}
	//on chat packet that is large enough
	//43 00 bb 0b 00 00 01 00 6f 84 9f 5d 00 00 00 00 97 09 0e 17 00 00 00 00 c5 02 00 00 00 00 00 00 01 00 11 00 54 75 64 69 20 73 65 63 6f 6e 64 61 00 0d 6a 63 77 00 00 05 09 00 31 32 33 34 35 36 37 38 39
	//Co]	Tudi secondajcw	123456789
	//3b 00 bb 0b 00 00 01 00 db 84 9f 5d 00 00 00 00 97 09 0e 17 00 00 00 00 c8 02 00 00 00 00 00 00 01 00 11 00 54 75 64 69 20 73 65 63 6f 6e 64 61 00 0d 6a 63 77 00 00 05 01 00 20
	if (len == 0x3b && packet[0] == 0x3b && packet[1] == 0 && packet[2] == 0xBB && packet[3] == 0x0B)
	{
		int ret = OnPacketForClickCastle(packet, len);
		if (ret == 0)
			return 0;
	}
	//we did not change the packet
	return 1;
}