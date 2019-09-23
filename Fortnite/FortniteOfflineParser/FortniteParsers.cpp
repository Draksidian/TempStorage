#include "FortniteDefines.h"
#include "StreamReader.h"
#include "FortniteParsers.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "StreamWriter.h"

int ForniteMetaParser::ParseMeta(StreamParser *sp, int PrintValues)
{
	unsigned int magic = sp->ReadUInt32();
	if (magic != FILE_MAGIC)
		printf("File magic was %08X was expecting %08X. Non critical error\n", magic, FILE_MAGIC);
	DebugPrint1(PrintValues, "Magic:%08X\n", magic);

	unsigned int FileVersion = sp->ReadUInt32();
	DebugPrint1(PrintValues, "FileVersion:%d\n", FileVersion);

	unsigned int LengthTime = sp->ReadUInt32();
	DebugPrint1(PrintValues, "LengthTime:%d\n", LengthTime);

	unsigned int NetworkVersion = sp->ReadUInt32();
	DebugPrint1(PrintValues, "NetworkVersion:%d\n", NetworkVersion);

	unsigned int ChangeList = sp->ReadUInt32();
	DebugPrint1(PrintValues, "ChangeList:%08X\n", ChangeList);

	unsigned char *FileString = sp->ReadWideString(); // should DUP it if we are going to use this later
	DebugPrint1(PrintValues, "FileString:%ws\n", (wchar_t *)FileString);

	unsigned int IsLive = sp->ReadUInt32();
	DebugPrint1(PrintValues, "IsLive:%d\n", IsLive);

	unsigned __int64 Timestamp = sp->ReadUInt64();
	DebugPrint1(PrintValues, "Timestamp:%lld\n", Timestamp);

	unsigned int Unk2 = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Unk2:%08X\n", Unk2);

	return 0;
}

int FortniteReadChuck::ParseChunk(StreamParser *sp, int PrintValues)
{
	chunk_type = sp->ReadUInt32();
	DebugPrint1(PrintValues, "chunk_type:%d\n", chunk_type);

	chunk_size = sp->ReadUInt32();
	DebugPrint2(PrintValues, "chunk_size: %d = %08X\n", chunk_size, chunk_size);

	return 0;
}

int ForniteHeaderParser::ParseHeader(StreamParser *sp, int ChunkSize, int PrintValues)
{
	int StartOffset = sp->GetOffset();

	unsigned int magic = sp->ReadUInt32();
	if (magic != NETWORK_MAGIC)
		printf("Network magic was %08X was expecting %08X. Non critical error\n", magic, NETWORK_MAGIC);
	DebugPrint1(PrintValues, "Magic:%08X\n", magic);

	unsigned int NetworkVersion = sp->ReadUInt32();
	DebugPrint1(PrintValues, "NetworkVersion:%d\n", NetworkVersion);

	unsigned int NetworkChecksum = sp->ReadUInt32();
	DebugPrint1(PrintValues, "NetworkChecksum:%d\n", NetworkChecksum);

	unsigned int EngineNetworkVersion = sp->ReadUInt32();
	DebugPrint1(PrintValues, "EngineNetworkVersion:%d\n", EngineNetworkVersion);

	unsigned int GameNetworkProtocol = sp->ReadUInt32();
	DebugPrint1(PrintValues, "GameNetworkProtocol:%d\n", GameNetworkProtocol);

	unsigned short GUID = sp->ReadUInt16();
	DebugPrint1(PrintValues, "GUID:%d\n", GUID);

	unsigned int major = sp->ReadUInt16();
	DebugPrint1(PrintValues, "major:%d\n", major);

	unsigned int minor = sp->ReadUInt16();
	DebugPrint1(PrintValues, "minor:%d\n", minor);

	unsigned int patch = sp->ReadUInt16();
	DebugPrint1(PrintValues, "patch:%d\n", patch);

	unsigned int changelist = sp->ReadUInt32();
	DebugPrint1(PrintValues, "changelist:%d\n", changelist);

	unsigned int Unk1 = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Unk1:%08X\n", Unk1);

	unsigned int Unk2 = sp->ReadUInt16();
	DebugPrint1(PrintValues, "Unk2:%04X\n", Unk1);

	unsigned int Unk3 = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Unk3:%08X\n", Unk3);

	unsigned int Unk4 = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Unk4:%08X\n", Unk4);

	unsigned char *branch = sp->ReadString(); // should DUP it if we are going to use this later
	DebugPrint1(PrintValues, "Version:%s\n", branch);

	unsigned int ArraySize = sp->ReadUInt32();
	DebugPrint1(PrintValues, "ArraySize:%d\n", ArraySize);

	for (unsigned int i = 0; i < ArraySize; i++)
	{
		unsigned char *VariableName = sp->ReadString();
		DebugPrint1(PrintValues, "VariableName:%s\n", VariableName);
		unsigned int VariableValue = sp->ReadUInt32();
		DebugPrint1(PrintValues, "ArrayValue:%d\n", VariableValue);
	}

	unsigned int flags = sp->ReadUInt32();
	DebugPrint1(PrintValues, "flags:%08X\n", flags);

	unsigned int Unk5 = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Unk5:%d\n", Unk5);

	unsigned char *VariableName = sp->ReadString();
	DebugPrint1(PrintValues, "VariableName:%s\n", VariableName);

	int EndOffset = sp->GetOffset();
	int BytesConsumed = EndOffset - StartOffset;
	assert(BytesConsumed <= ChunkSize);
	if (BytesConsumed < ChunkSize)
		printf("Skipping %d bytes from header\n", ChunkSize - BytesConsumed);


	return 0;
}

int ForniteEventParser::ParseEvent(StreamParser *sp, int ChunkSize, int PrintValues)
{
	int StartOffset = sp->GetOffset();

	unsigned char *event_id = sp->ReadString();
	DebugPrint1(PrintValues, "event_id:%s\n", event_id);
	unsigned char *group = sp->ReadString();
	DebugPrint1(PrintValues, "group:%s\n", group);
	unsigned char *metadata = sp->ReadString();
	DebugPrint1(PrintValues, "metadata:%s\n", metadata);
	unsigned int StarTime = sp->ReadUInt32();
	DebugPrint1(PrintValues, "StarTime:%d\n", StarTime);
	unsigned int EndTime = sp->ReadUInt32();
	DebugPrint1(PrintValues, "EndTime:%d\n", EndTime);
	unsigned int Size = sp->ReadUInt32();
	DebugPrint1(PrintValues, "Size:%d\n", Size);
	assert((int)Size < ChunkSize);

	if (!(strcmp("versionedEvent", (char*)metadata) == 0 || strcmp("PlayerStateEncryptionKey", (char*)metadata) == 0 || strcmp("AthenaMatchTeamStats", (char*)metadata) == 0 || strcmp("AthenaMatchStats", (char*)metadata) == 0) )
	{
		printf("Unhandled meta data type\n");
	}

	if (!(strcmp("playerElim", (char*)group) == 0 || strcmp("AthenaReplayBrowserEvents", (char*)group) == 0 || strcmp("PlayerStateEncryptionKey", (char*)group) == 0))
	{
		printf("Unhandled group data type\n");
	}

	int HandledParsing = 0;
	if (strcmp(EV_PLAYER_ELIMINATION, (char*)group) == 0)
	{
		sp->SkipBytes(87);
		int UUIDOffset = sp->GetOffset();

		unsigned char *UIDKIller = sp->GetByteArray();
		DebugPrint0(PrintValues, "GUID1:");
		for (int i = 0; i < 16; i++)
		{
			int Val = sp->ReadUInt8();
			DebugPrint1(PrintValues, "%02x", Val);
		}
		DebugPrint0(PrintValues, "\n");
		sp->SkipBytes(2);

		unsigned char *UIDKIlled = sp->GetByteArray();
		DebugPrint0(PrintValues, "GUID2:");
		for (int i = 0; i < 16; i++)
		{
			int Val = sp->ReadUInt8();
			DebugPrint1(PrintValues, "%02x", Val);
		}
		DebugPrint0(PrintValues, "\n");

		unsigned char gun_type = sp->ReadUInt8();
		DebugPrint1(PrintValues, "gun_type:%d\n", gun_type);
		unsigned int knocked = sp->ReadUInt32();
		DebugPrint1(PrintValues, "knocked:%d\n", knocked);

		GetWriter().AddJsonKill(UIDKIller, UIDKIlled, knocked, gun_type);
		GetWriter().Flush();

		//			sp->SkipBytes(Size);
		HandledParsing = 1;
	}
	else if (strcmp(EV_MATCH_STATS, (char*)metadata) == 0)
	{
		unsigned int Unk1 = sp->ReadUInt32();
		DebugPrint1(PrintValues, "Unk1:%d\n", Unk1);
		float accuracy = sp->ReadFloat();
		DebugPrint1(PrintValues, "accuracy:%f\n", accuracy);
		unsigned int assists = sp->ReadUInt32();
		DebugPrint1(PrintValues, "assists:%d\n", assists);
		unsigned int eliminations = sp->ReadUInt32();
		DebugPrint1(PrintValues, "eliminations:%d\n", eliminations);
		unsigned int weapon_damage = sp->ReadUInt32();
		DebugPrint1(PrintValues, "weapon_damage:%d\n", weapon_damage);
		unsigned int other_damage = sp->ReadUInt32();
		DebugPrint1(PrintValues, "other_damage:%d\n", other_damage);
		unsigned int revives = sp->ReadUInt32();
		DebugPrint1(PrintValues, "revives:%d\n", revives);
		unsigned int damage_taken = sp->ReadUInt32();
		DebugPrint1(PrintValues, "damage_taken:%d\n", damage_taken);
		unsigned int damage_structures = sp->ReadUInt32();
		DebugPrint1(PrintValues, "damage_structures:%d\n", damage_structures);
		unsigned int materials_gathered = sp->ReadUInt32();
		DebugPrint1(PrintValues, "materials_gathered:%d\n", materials_gathered);
		unsigned int materials_used = sp->ReadUInt32();
		DebugPrint1(PrintValues, "materials_used:%d\n", materials_used);
		unsigned int total_traveled = sp->ReadUInt32();
		DebugPrint1(PrintValues, "total_traveled:%d\n", total_traveled);
		HandledParsing = 1;
	}

	if (strcmp(EV_TEAM_STATS, (char*)metadata) == 0)
	{
		unsigned int unknown = sp->ReadUInt32();
		DebugPrint1(PrintValues, "unknown:%d\n", unknown);
		unsigned int position = sp->ReadUInt32();
		DebugPrint1(PrintValues, "position:%d\n", position);
		unsigned int total_players = sp->ReadUInt32();
		DebugPrint1(PrintValues, "total_players:%d\n", total_players);
		HandledParsing = 1;
	}

	if (HandledParsing == 0)
	{
		printf("Unknown event type found : %s\n", group);
		sp->SkipBytes(Size);
	}

	int EndOffset = sp->GetOffset();
	int BytesConsumed = EndOffset - StartOffset;
	assert(BytesConsumed <= ChunkSize);
	if (BytesConsumed < ChunkSize)
		printf("Skipping %d bytes from header\n", ChunkSize - BytesConsumed);


	return 0;
}
