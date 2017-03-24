#pragma once


#define MAX_KNOWN_REALM_ID		120		//actually it's only 100, but maybe they will add more very soon
#define MAX_KNOWN_CASTLE_LEVEL	25

enum ObjectTypesList
{
	OBJECT_TYPE_MAYBE_ARMY		= 0,	// don't think it's army, got it for neighbour that had no army or camp out
	OBJECT_TYPE_RESOURCE_FOOD	= 1,
	OBJECT_TYPE_RESOURCE_ROCK	= 2,
	OBJECT_TYPE_RESOURCE_ORE	= 3,
	OBJECT_TYPE_RESOURCE_WOOD	= 4,
	OBJECT_TYPE_RESOURCE_GOLD	= 5,
	OBJECT_TYPE_GEM_RESOURCE	= 6,
	OBJECT_TYPE_PLAYER			= 8,
	OBJECT_TYPE_CAMP			= 9,
	OBJECT_TYPE_MONSTER			= 10,	//
	OBJECT_TYPE_MAX_KNOWN
};

enum PlayerCastleStatusFlags
{
	CASTLE_STATUS_BURNING		= 0x02,
	CASTLE_STATUS_SHIELDED		= 0x04,
	CASTLE_STATUS_HAS_PRISONERS = 0x08,
};

enum PlayerCastleTitles
{
	TITLE_QUARTERMASTER		= 4,
	TITLE_SCOUNDREL			= 13,
};

enum MonsterTypes
{
	MONSTER_MECHA_TROJAN = 19,
	MONSTER_BLACKWING_OR_BON_APETI1 = 16,
	MONSTER_BLACKWING_OR_BON_APETI2 = 18,
};

#pragma pack(push, 1)
struct PlayerNameExt
{
	unsigned char	StatusFlags;
	unsigned short	Title;
	unsigned short	RealmGuild;
	unsigned int	ExtendedTypeId;			//always 0 except for dark.nest ( has 513 )? Maybe IconId ?
};
struct MineExt
{
	unsigned int	ResourceMax;
	float			MinedPercent;
	unsigned int	SomeTimestamp;		// not confirmed
};
#define MAX_BUILDING_NAME 13
#define MAX_GUILD_SHORT_NAME 3
struct BuildingObjectDesciption
{
	//	unsigned int	GUID;
	//	unsigned char	ObjectType;				//dark nest and castle are both 8. Maybe struct type ( x bytes in format .. )
	char			Name[MAX_BUILDING_NAME];
	char			Guild[MAX_GUILD_SHORT_NAME];
	unsigned short	Realm;
	unsigned char	CastleLevel;
	union {
		PlayerNameExt	PEx;
		MineExt			MEx;
		char			PaddingOrSomething[19];
	};
};
struct MonsterObjectDescriptor
{
	unsigned char	Level;
	unsigned short	Type;
	unsigned int	Time;					//seconds multiplied by 100 ? Or milliseconds ? Have to confirm
	float			HealthPCT;
};
struct GenericMapObject
{
	unsigned int	GUID;
	unsigned char	ObjectType;				//dark nest and castle are both 8. Maybe struct type ( x bytes in format .. )
	union{
		BuildingObjectDesciption	B;
		MonsterObjectDescriptor		M;
	};
};
#define MAX_GUILD_FULL_NAME 20
struct CastlePopupInfo
{
	__int64			TileID; //??? guessing
	unsigned char	unk2F;	//seems to be always 0x2F
	unsigned int	GUID;	// the guid we clicked on
	char			GuildFullName[MAX_GUILD_FULL_NAME];
	unsigned char	VIPLevel;
	unsigned char	GuildRank;
	unsigned char	Unk[6];
	__int64			Might;
	__int64			Kills;
	//12 more unk bytes
};
#pragma pack(pop)

void ParseOfflineDump(const char *FileName);
void ProcessPacket1(unsigned char *packet, int size);
void ProcessPacketDebug(char *HexStr);

#define MAX_PACKET_CIRCULAR_BUFFER	10000
void	CreateBackgroundPacketProcessThread();
void	QueuePacketToProcess(unsigned char *data, int size);
void	StopThreadedPacketParser();