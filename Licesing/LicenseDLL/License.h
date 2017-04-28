#pragma once

#define CURENT_LICENSE_NODE_VER				1
#define ERROR_INVALID_PROJECT_FEAURE		(-2)
#define WARNING_NO_LICENSE_FOUND			(-3)
#define WARNING_MISSING_LICENSE_FIELD		(-4)
#define ERROR_LICENSE_EXPIRED				(-5)
#define ERROR_COULD_NOT_LOAD_LIC_FILE		(-6)
#define ERROR_COULD_NOT_STORE_GRACE_PERIOD	(-7)
#define ERROR_LICENSE_INVALID				(-8)
#define ERROR_MISSING_PARAMETER				(-9)
#define ERROR_DISABLED_FUNCTION				(-10)
#define ERROR_MISSING_ENCRYPTION_KEY		(-11)

#ifdef _DEBUG
	#define LICENSE_DURATION_UPDATE_INTERVAL	( 2000 )	// trigger lots of events to not need to wait for testing. Needs to be at least 1 second
#else
	#define LICENSE_DURATION_UPDATE_INTERVAL	( 5 * 60 * 1000 )
#endif

#define GenerateSaltKey(a,b)	((unsigned int)(1|(a+b)))
#define GenerateSaltKey2		(GenerateSaltKey( GetTickCount(), time(NULL) ))
#define GenerateSaltKey3(i,j)	( GetTickCount() | ( GetTickCount() << ((0+i)*8+j)) | ( GetTickCount() << ((1+i)*8+j)) | ( GetTickCount() << ((2+i)*8+j)))

#define HardCodedStringLimitLic 50

#define ENABLE_ANTI_HACK_CHECKS

#define MAX_LICENSE_SIZE_BYTES				(100*1024*1024)
#define PERMANENT_LICENSE_MAGIC_DURATION	(~((unsigned int)0))

#ifndef min
    #define min(a,b) (a>b?a:b)
#endif

#pragma pack(push,1)
class LicenseNode
{
public:
	int		GetProjectId() { return ProjectId; }
	void	SetProjectId(int pProjectId) { ProjectId = pProjectId; }
	int		GetFeatureId() { return FeatureId; }
	void	SetFeatureId(int pFeatureId) { FeatureId = pFeatureId; }
	size_t	GetKeyLen() { return min(HardCodedStringLimitLic, strlen(ActivateKey1)); }
	int		GetKey(char *Store, int MaxStore)
	{
		if (Store == NULL || MaxStore == 0)
			return 1;
		int Ind = 0;
		while (Ind < MaxStore - 1 && ActivateKey1[Ind] != 0)
		{
			Store[Ind] = ActivateKey1[Ind] + ActivateKey2[Ind] - 11;
			Ind++;
		}
		Store[Ind] = 0;
		return 0;
	}
	//we could simply copy the string. Making store complicated to avoid humanly readable format. We could use some simple xorkey, but reading from 2 addresses is more complicated to debug
	void	SetKey(const char *pKey)
	{
		if (pKey == NULL)
			return;
		int Ind = 0;
		while (Ind < HardCodedStringLimitLic - 1 && pKey[Ind] != 0)
		{
			char a = pKey[Ind] ^ ( (( Ind + 3 ) % pKey[Ind]) >> 1 );
			char b = 11 + pKey[Ind] - a; // should be always greateer than 0
			ActivateKey1[Ind] = b;
			ActivateKey2[Ind] = a;
			Ind++;
		}
		ActivateKey1[Ind] = 0;
		ActivateKey2[Ind] = 0;
	}
private:
	int		ProjectId;			// get the list from static files
	int		FeatureId;			// get the list from static files
	char	ActivateKey1[HardCodedStringLimitLic];
	char	ActivateKey2[HardCodedStringLimitLic];
};
#pragma pack(pop)

class GenericDataStore;

//save status. Might be used at some point to investigate hacking
enum LicenseStateFlags
{
	LSF_LOADED_FROM_FILE		= 1,
	LSF_DURATION_IS_SET			= 2,		// should never be present in client side logs
	LSF_FEATURE_ADDED			= 4,		// should never be present in client side logs
	LSF_FEATURE_REQUESTED		= 8,
	LSF_SAVED_TO_FILE			= 16,		// should never be present in client side logs
	LSF_DURATION_REQUESTED		= 32,		// should never be present in client side logs
	LSF_GRACE_STATUS_QUERIED	= 64,
	LSF_SIEMENS_ONLY_FLAGS		= (LSF_DURATION_IS_SET | LSF_FEATURE_ADDED | LSF_SAVED_TO_FILE),
};

class LIBRARY_API License
{
public:
	License();
	~License();
	int					SetDuration(time_t pStartDate, unsigned int pDuration, unsigned int pGraceDuration);	// duration values are in seconds
//	int					AddProjectFeature(const char *ProjectName, const char *FeatureName);
//	int					AddProjectFeature(int ProjectId, int FeatureId);
	int					AddProjectFeature(int ProjectId, int FeatureId, const char *ActivationKey);
	int					SaveToFile(const char *FileName, const char *FingerprintFilename, int Append=0);		// Save License to a file that can be sent to Siemens licensing team
	int					SaveToFile(const char *FileName, const char *Key, const int KeyLen, int Append);	
	int					LoadFromFile(const char *FileName);															// load license from a file that can be sent to Siemens licensing team
	int					LoadFromFile(const char *FileName, char *FingerprintFilename);								// load a license while not using a local fingerprint file. This should only be used by LicenseInfo Project
	int					GetActivationKey(int ProjectId, int FeatureId, char *StoreResult, int MaxResultSize);	// API to check if a project-feature is usable based on the license file we loaded
	int					GetRemainingSeconds(time_t *RemainingSeconds);											// this does not include grace period
	int					IsGracePeriodTriggered(time_t *RemainingSeconds);										// check if grace period is getting used right now
private:
	int					InitFromBuffer(char *buf, int Len, unsigned int EncryptSalt, char *key, int KeyLen);	// init internal states from a buffer fetched either from file or DB
	time_t				StartStamp;
	unsigned int		Duration;
	time_t				GracePeriod;
	GenericDataStore	*NodeList;
	int					UsageStateFlags;	// logs and more logs about license usage. 
	int					DisableEdit;		// minor protection to deny API usage to edit a license if it was loaded from a file
	int					DisableQuery;		// minor protection to deny API calls on a newly constructed license
};

#ifdef _DEBUG
	extern int ENABLE_ERROR_PRONE_TESTS;
	LIBRARY_API void EnableErrorTests();
#endif
