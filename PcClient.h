/*
 * MacroQuest: The extension platform for EverQuest
 * Copyright (C) 2002-present MacroQuest Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#pragma once

#include "Common.h"
#include "Constants.h"
#include "Containers.h"
#include "CXStr.h"
#include "Achievements.h"
#include "Items.h"
#include "PlayerClient.h"
#include "Spells.h"
#include "EQData.h"
#include "PcProfile.h"

// This is the home of all things related to what used to be called CHARINFO 
// plus a few extra things until they find a home of their own.
//
// Class hierarchy:
//
// CHARINFO:
//   PcClient -> PcZoneClient -> PcBase, CharacterZoneClient, [SomeOtherClass]
//   PcBase -> CharacterBase


namespace eqlib {

class CharacterBase;
class CharacterZoneClient;
class HateListEntry;
class PcBase;
class PcClient;
class PcZoneClient;
class ProfileManager;

//============================================================================
// Enums
//============================================================================

enum EAreaCorner
{
	eAC_None = -1,
	eAC_TopLeftCorner,
	eAC_TopRightCorner,
	eAC_BottomLeftCorner,
	eAC_BottomRightCorner,
};

enum ELockoutCharacterReason
{
	LCR_AllowNormalPlay,
	LCR_InvalidWornItem,
	LCR_Something,
};

enum eXTSlotStatus
{
	eXTSlotEmpty,
	eXTSlotCurrentZone,
	eXTSlotDifferentZone,
	eXTSlotUnknown
};

constexpr int MAX_BLOCKED_SPELLS = 60;
constexpr int MAX_BLOCKED_SPELLS_PET = 60;

constexpr int EQSKILL_HIDE = 29;

//============================================================================
// Structs
//============================================================================

struct [[offsetcomments]] ALCHEMYBONUSSKILLDATA
{
	FORCE_SYMBOLS;

/*0x00*/ int SkillID;
/*0x04*/ int BonusPoints;
/*0x08*/
};

enum eGroupRoles
{
	GroupRoleNone,
	GroupRoleTank,
	GroupRoleAssist,
	GroupRolePuller,
	GroupRoleMarkNPC,
	GroupRoleMasterLooter,

	MaxGroupRoles
};

//============================================================================
// CGroupMember
//============================================================================

// Real name is CGroupMemberClient, but we will use CGroupMember for short since
// we only ever deal with the client.
class CGroupMember;

class [[offsetcomments]] CGroupMemberBase
{
public:
/*0x08*/ CXStr                 Name;
/*0x10*/ short                 Type;                                 // player type (EQP_PC, EQP_NPC, etc)
/*0x18*/ CXStr                 OwnerName;
/*0x20*/ int                   Level;
/*0x24*/ bool                  bIsOffline;
/*0x28*/ uint32_t              UniquePlayerID;
/*0x2c*/ bool                  bRoleStates[MaxGroupRoles];
/*0x34*/ uint32_t              CurrentRoleBits;                      // (Roles & 0x1) = MainTank, 0x2 = MainAssist, 0x4 = Puller 0x8 = Mark NPC 0x10 = Master Looter
/*0x38*/ eqtime_t              OnlineTimestamp;
/*0x40*/

	CGroupMemberBase();
	virtual ~CGroupMemberBase();
	virtual void* GetUnknownPtr() { return nullptr; }
	virtual void* GetUnknownPtr2() { return nullptr; }
	virtual void* GetUnknownPtr3() { return nullptr; }
	virtual CharacterZoneClient* GetCharacter() { return nullptr; }
	virtual CGroupMember* AsMemberClient() { return nullptr; }
	virtual void RemovedFromGroup(uint32_t id) = 0;

	inline bool IsOffline() const { return bIsOffline; }
	inline eqtime_t GetOnlineTimestamp() const { return OnlineTimestamp; }
	inline bool GetRole(eGroupRoles role) const { return bRoleStates[role]; }
	inline const char* GetName() const { return Name.c_str(); }
	inline const char* GetOwnerName() const { return OwnerName.c_str(); }
	inline int GetLevel() const { return Level; }

	inline bool IsMainTank() const { return GetRole(GroupRoleTank); }
	inline bool IsMainAssist() const { return GetRole(GroupRoleAssist); }
	inline bool IsPuller() const { return GetRole(GroupRolePuller); }
	inline bool IsMarkNPC() const { return GetRole(GroupRoleMarkNPC); }
	inline bool IsMasterLooter() const { return GetRole(GroupRoleMasterLooter); }

	// Compat wrappers for old member types/names
	__declspec(property(get = getPName)) CXStr* pName;
	DEPRECATE("CGroupMemberBase: Use Name instead of pName")
	inline CXStr* getPName() { return &Name; }

	__declspec(property(get = getPOwner)) CXStr* pOwner;
	DEPRECATE("CGroupMemberBase: Use OwnerName instead of pOwner")
	inline CXStr* getPOwner() { return &OwnerName; }

	// Compat wrapper for Mercenary
	__declspec(property(get = getMercenary)) uint8_t Mercenary;
	DEPRECATE("CGroupMemberBase: Use Type instead of Mercenary")
	inline uint8_t getMercenary() { return (uint8_t)Type; }

	ALT_MEMBER_GETTER(bool, bIsOffline, Offline);
	ALT_MEMBER_GETTER(uint32_t, CurrentRoleBits, Roles);

	__declspec(property(get = IsMainTank)) bool MainTank;
	__declspec(property(get = IsMainAssist)) bool MainAssist;
	__declspec(property(get = IsPuller)) bool Puller;
	__declspec(property(get = IsMarkNPC)) bool MarkNpc;
	__declspec(property(get = IsMasterLooter)) bool MasterLooter;

private:
	void ClearRoles();
};

class [[offsetcomments]] CGroupMember : public CGroupMemberBase
{
public:
/*0x40*/ CharacterZoneClient*  pCharacter;
/*0x48*/ PlayerClient*         pPlayer;
/*0x50*/ int                   GroupIndex;
/*0x54*/

	CGroupMember();
	virtual ~CGroupMember();

	virtual CharacterZoneClient* GetCharacter() override { return pCharacter; }
	virtual CGroupMember* AsMemberClient() override { return this; }
	PlayerClient* GetPlayer() { return pPlayer; }

	ALT_MEMBER_GETTER(PlayerClient*, pPlayer, pSpawn);
};


inline namespace deprecated {
	using GROUPMEMBER DEPRECATE("Use CGroupMember instead of GROUPMEMBER") = CGroupMember;
	using PGROUPMEMBER DEPRECATE("Use CGroupMember* instead of PGROUPMEMBER") = CGroupMember*;
}

//============================================================================
// CGroupClient
//============================================================================

// This class holds information about the members of the group. Technically it
// stores CGroupMemberBase, but since this is the client, these will always be
// instances of CGroupMember, so we'll just skip the type casts and use
// the type instead.

class [[offsetcomments]] CGroupBase
{
public:
	CGroupBase();
	virtual ~CGroupBase();
	// other virtuals not mapped out

	CGroupMember* GetGroupLeader() const { return m_groupLeader; }
	EQLIB_OBJECT CGroupMember* GetGroupMember(int index) const;

	inline uint32_t GetID() const { return m_id; }

	// iterator support for stl containers and algorithms
	inline auto begin() { return std::begin(m_groupMembers); }
	inline auto begin() const { return std::cbegin(m_groupMembers); }
	inline auto cbegin() const { return std::cbegin(m_groupMembers); }
	inline auto end() { return std::end(m_groupMembers); }
	inline auto end() const { return std::cend(m_groupMembers); }
	inline auto cend() { return std::cend(m_groupMembers); }

	ALT_MEMBER_GETTER_ARRAY_DEPRECATED(CGroupMember*, MAX_GROUP_SIZE, m_groupMembers, pMember,
		"CGroupBase: Use Group->GetGroupMember instead of accessing pMembers");
	ALT_MEMBER_GETTER(CGroupMember*, m_groupLeader, pLeader);

protected:
/*0x08*/ CGroupMember*         m_groupMembers[MAX_GROUP_SIZE];
/*0x38*/ CGroupMember*         m_groupLeader;
/*0x40*/ uint32_t              m_id;
/*0x44*/
};

class [[offsetcomments]] CGroup : public CGroupBase
{
public:
	using CGroupBase::GetGroupMember;

	// Get the mercenary member for the given owner name.
	EQLIB_OBJECT CGroupMember* GetMercenary(std::string_view ownerName) const;

	// Get the group member for the given name or index
	EQLIB_OBJECT CGroupMember* GetGroupMember(std::string_view name) const;

	// Retrieve a group member by some criteria
	EQLIB_OBJECT CGroupMember* GetGroupMember(PlayerClient* pPlayer) const;
	EQLIB_OBJECT CGroupMember* GetGroupMemberByRole(eGroupRoles role) const;

	// Retrieve a group member by its position (skipping holes)
	EQLIB_OBJECT CGroupMember* GetNthGroupMember(int position) const;

	// Get the number of group members
	EQLIB_OBJECT uint32_t GetNumberOfMembers(bool includeOffline = true) const;
	EQLIB_OBJECT uint32_t GetNumberOfPlayerMembers(bool includeOffline = true) const;
	EQLIB_OBJECT uint32_t GetNumberOfMembersExcludingSelf(bool includeOffline = true) const;

	// What is the index of this group member? Returns -1 if not found. Returns ARRAY INDEX. (counts blank slots)
	EQLIB_OBJECT int GetGroupMemberIndex(CGroupMember* pMember) const;

	// Returns the VISUAL INDEX of the group member. That is, what position they are in the list, ignoring blank slots.
	EQLIB_OBJECT int GetGroupMemberVisualIndex(CGroupMember* pMember) const;

	EQLIB_OBJECT bool IsGroupMember(PlayerClient* pPlayer) const;
	EQLIB_OBJECT bool IsGroupLeader(PlayerClient* pPlayer) const;

private:
/*0x48*/ int                   GroupSelectID;
/*0x4c*/
};

inline namespace deprecated {
	using GROUPINFO DEPRECATE("Use CGroup instead of GROUPINFO") = CGroup;
	using PGROUPINFO DEPRECATE("Use CGroup* instead of PGROUPINFO") = CGroup*;
}
using CGroupClient = CGroup;

//============================================================================
//============================================================================

// size 0x4c 12-25-09 - ieatacid
struct [[offsetcomments]] ExtendedTargetSlot
{
/*0x00*/ DWORD          xTargetType;
/*0x04*/ eXTSlotStatus  XTargetSlotStatus;
/*0x08*/ uint32_t       SpawnID;
/*0x0c*/ char           Name[EQ_MAX_NAME];
/*0x4c*/
};

inline namespace deprecated {
	using XTARGETSLOT DEPRECATE("Use ExtendedTargetSlot instead of PXTARGETSLOT") = ExtendedTargetSlot;
	using PXTARGETSLOT DEPRECATE("Use ExtendedTargetSlot* instead of XTARGETSLOT") = ExtendedTargetSlot*;
}

DEPRECATE("The number of extended targets is not hard coded. Use GetNumSlots() to get the right value.")
constexpr int MAX_EXTENDED_TARGETS  = 23;

class [[offsetcomments]] ExtendedTargetList
{
public:
	using TargetSlotArray = ArrayClass<ExtendedTargetSlot>;

	int GetNumSlots() const { return m_targetSlots.GetLength(); }

	// will return null if slot is out of bounds. If not performing a bounds check,
	// its probably a good idea to do a null check on the response.
	EQLIB_OBJECT ExtendedTargetSlot* GetSlot(int slot);

	bool GetAutoAddHaters() const { return m_autoAddHaters; }
	virtual void SetAutoAddHaters(bool autoAdd) { m_autoAddHaters = autoAdd; }

	auto begin() { return m_targetSlots.begin(); }
	auto cbegin() const { return m_targetSlots.cbegin(); }
	auto end() { return m_targetSlots.end(); }
	auto cend() const { return m_targetSlots.cend(); }

	EQLIB_OBJECT const char* ExtendedTargetRoleName(uint32_t xTargetType);

private:
/*0x08*/ TargetSlotArray m_targetSlots;
/*0x20*/ bool m_autoAddHaters;
/*0x24*/ int CurrentSlot;
/*0x28*/ int ContextSlot;
/*0x2c*/

public:
	ALT_MEMBER_GETTER(TargetSlotArray, m_targetSlots, XTargetSlots);
	ALT_MEMBER_GETTER(bool, m_autoAddHaters, bAutoAddHaters);
};

EQLIB_API DEPRECATE("GetXtargetType() is deprecated. Use pLocalPC->pExtendedTargetList->ExtendedTargetRoleName() instead")
const char* GetXtargetType(DWORD type);

struct [[offsetcomments]] MailItemData
{
/*0x00*/ UINT   SendTime;
/*0x08*/ CXStr  SenderName;
/*0x10*/ CXStr  Note;
/*0x18*/
};

struct [[offsetcomments]] PCAdventureThemeStats
{
/*0x00*/ int SucceededNormal;
/*0x04*/ int FailedNormal;
/*0x08*/ int SucceededHard;
/*0x0c*/ int FailedHard;
/*0x10*/ int AdventureTotalPointsEarned;
/*0x14*/
};

struct [[offsetcomments]] PCAdventureData
{
/*0x00*/ int                   AdventureLastAdventureDefinitionSeen[5];
/*0x18*/ eqtime_t              AdventureLastAdventureDefinitionSeenTime[5];
/*0x40*/ int                   AdventureActiveAdventureId;
/*0x44*/ int                   AdventureActiveAdventureTheme;
/*0x48*/ int                   AdventureActiveAdventureRisk;
/*0x4c*/ int                   AdventureSafeReturnZoneId;
/*0x50*/ float                 AdventureSafeReturnX;
/*0x54*/ float                 AdventureSafeReturnY;
/*0x58*/ float                 AdventureSafeReturnZ;
/*0x5c*/ int                   AdventureStatAccepted;
/*0x60*/ int                   AdventureStatRejected;
/*0x64*/ int                   AdventureStatEntered;
/*0x68*/ int                   AdventureStatFailedEnter;
/*0x6c*/ int                   AdventurePointsAvailable;
/*0x70*/ int                   AdventurePointsAvailableMax;
/*0x78*/ eqtime_t              AdventureLastSuccessTime;
/*0x80*/ PCAdventureThemeStats ThemeStats[6];
/*0xf8*/
};

struct [[offsetcomments]] PCTaskStatus
{
/*0x00*/ int  TaskID;
/*0x04*/ int  MovingStartTime;
/*0x08*/ int  InitialStartTime;
/*0x0c*/ bool ElementActive[0x14];
/*0x20*/ int  CurrentCounts[0x14];
/*0x70*/
};

struct [[offsetcomments]] MonsterMissionTemplate
{
/*0x00*/ int  TemplateID;
/*0x04*/ int  Min;
/*0x08*/ int  Max;
/*0x0c*/ int  NumSelected;
/*0x10*/ bool CanSelect;
/*0x11*/ char TemplateName[0x40];
/*0x54*/
};

struct [[offsetcomments]] PCSharedTaskData
{
/*0x00*/ int          ActiveSharedTaskID;
/*0x04*/ bool         bIsMonsterMission;
/*0x08*/ PCTaskStatus Status;                              // size 0x70
/*0x78*/ ArrayClass<MonsterMissionTemplate> Templates;  // size is 0x10
/*0x90*/ float        RewardAdjustment;
/*0x94*/
};

struct [[offsetcomments]] TaskTimerData
{
/*0x00*/ int GroupID;
/*0x04*/ int TimerSeconds;
/*0x08*/ UINT TimerExpiration;
/*0x0c*/ int TimerType;
/*0x10*/ int OrigTaskID;
/*0x18*/ TaskTimerData *pNext;
/*0x20*/
};

class [[offsetcomments]] PendingReward
{
	FORCE_SYMBOLS;

public:
/*0x00*/ void* vfTable;
/*0x08*/ int   ID;
/*0x0c*/ int   SetID;
/*0x10*/ float RewardAdjustment;
/*0x14*/ int   RewardProfileId;
/*0x18*/ char  RewardTitle[0x80];
/*0x98*/
};

class [[offsetcomments]] PendingRewardList : public DoublyLinkedList<PendingReward*>
{
public:
/*0x38*/ int NextUID;
/*0x3c*/ int MaxPending;
/*0x40*/ int ZoneMaxPending;
/*0x44*/
};

struct [[offsetcomments]] Point
{
/*0x00*/ UINT PointType;
/*0x04*/ UINT PointSubtype;
/*0x08*/ UINT CurrentCount;
/*0x0c*/ UINT TotalEver;
/*0x10*/
};

struct [[offsetcomments]] PointNamesEntry
{
/*0x00*/ UINT PointTypeId;
/*0x04*/ UINT PointSubtypeId;
/*0x08*/ int  DBStringId;
/*0x0c*/ int  PointItemId;
/*0x10*/ int  ImageId;
/*0x14*/ int  MaxStackSize;
/*0x18*/ bool bStationCashRelated;
/*0x1c*/
};

class [[offsetcomments]] PointSystemBase
{
public:
/*0x00*/ ArrayClass<PointNamesEntry*> PointNameEntries;
/*0x18*/ SoeUtil::String str;
/*0x30*/
};

class [[offsetcomments]] CPlayerPointManager
{
public:
	EQLIB_OBJECT unsigned long GetAltCurrency(unsigned long, unsigned long b = 1);

/*0x00*/ void* vfTable;
/*0x08*/ ArrayClass<Point*> Points;
/*0x20*/
};
using PlayerPointManager = CPlayerPointManager;

struct [[offsetcomments]] ProgressionExperience
{
/*0x00*/ int ProgressionID;
/*0x08*/ double ProgressionExp;
/*0x10*/
};

struct [[offsetcomments]] PCCompletedQuest
{
/*0x00*/ int        QuestID;
/*0x04*/ int        ElementBitmask;
/*0x08*/ eqtime_t   TimeCompleted;
/*0x10*/
};

struct [[offsetcomments]] PCQuestHistoryData
{
/*0x000*/ PCCompletedQuest Quests[50];
/*0x320*/
};

struct [[offsetcomments]] PvPKill
{
/*0x00*/ char     VictimName[EQ_MAX_NAME];
/*0x40*/ int      VictimLevel;
/*0x44*/ int      Unknown0x44;
/*0x48*/ int      Unknown0x48;
/*0x4c*/ int      VictimRace;
/*0x50*/ int      VictimClass;
/*0x54*/ int      ZoneID;
/*0x58*/ int      Lastkilltime;
/*0x5c*/ int      PointsEarned;
/*0x60*/
};

struct [[offsetcomments]] PvPDeath
{
/*0x00*/ char     KillerName[EQ_MAX_NAME];
/*0x40*/ int      KillerLevel;
/*0x44*/ int      KillerRace;
/*0x48*/ int      KillerClass;
/*0x4c*/ int      ZoneID;
/*0x50*/ int      LastDeathTime;
/*0x54*/ int      PointsLost;
/*0x58*/
};

struct PvPKill24HourData : public PvPKill
{
	//nothing here?
};

struct [[offsetcomments]] TradeskillRecipeCount
{
	FORCE_SYMBOLS;

/*0x00*/ int SkillID;
/*0x04*/ int RecipeCount;
/*0x08*/
};

class [[offsetcomments]] StatCounter
{
public:
/*0x00*/ uint64_t Value;
/*0x08*/
};

class [[offsetcomments]] StatElapsedTime
{
public:
/*0x00*/ eqtime_t StartTick;
/*0x08*/ uint32_t ElapsedTotal;
/*0x0c*/ bool     bCurrentState;
/*0x10*/
};

class [[offsetcomments]] PCStatistics
{
public:
	enum eStatisticType
	{
		S_TotalExpEarned,
		S_GroupExpEarned,
		S_ExpRaidEarned,
		S_ExpSoloEarned,
		S_NonExpKills,
		S_ExpKills,
		S_ZonesVisited,
		S_ChatShouts,
		S_ChatOOCs,
		S_ChatSays,
		S_ChatGroup,
		S_ChatTells,
		S_Deaths,
		S_Resurrections,
		S_PlatEarned,
		S_TradeskillCombines,
		S_Forages,
		S_Quests,
		S_LastStat,
	};

/*0x000*/ StatElapsedTime StatTimeSession;
/*0x010*/ StatElapsedTime StatTimeLFG;
/*0x020*/ StatElapsedTime StatTimeGrouped;
/*0x030*/ StatElapsedTime StatTimeSolo;
/*0x040*/ StatElapsedTime StatTimeRaid;
/*0x050*/ StatElapsedTime StatTimeInBazaar;
/*0x060*/ StatCounter     Statistics[S_LastStat];
/*0x0f0*/ eqtime_t        LastUpdateTime;
/*0x0f8*/ char            PlayerName[EQ_MAX_NAME];
/*0x138*/ char            PlayerStationID[EQ_MAX_STATION_ID];
/*0x158*/ int             PlayerLevel;
/*0x15c*/ int             PlayerRace;
/*0x160*/ int             PlayerClass;
/*0x164*/ uint32_t        UniquePlayerID;
/*0x168*/
};

class [[offsetcomments]] GroupMemberStats
{
public:
	enum eStatisticType
	{
		S_Mez,
		S_Root,
		S_Charmed,
		S_Stunned,
		S_Slowed,
		S_FirstAgro,
		S_DmgMelee,
		S_DmgRanged,
		S_DmgSpell,
		S_DmgDot,
		S_DmgPet,
		S_DmgTaken,
		S_DmgHealed,
		S_ExpTotalEarned,
		S_Deaths,
		S_ExpKills,
		S_NonExpKills,
		S_ManaUsed,
		S_EnduranceUsed,
		S_LastStat,
	};

/*0x000*/ char         PlayerName[EQ_MAX_NAME];
/*0x040*/ int          PlayerLevel;
/*0x044*/ int          PlayerRace;
/*0x048*/ int          PlayerClass;
/*0x04c*/ uint32_t     UniquePlayerId;
/*0x050*/ uint32_t     LastMemberUpdateTime;
/*0x058*/ CXStr        StationID;
/*0x060*/ int64_t      PlayerGuild;
/*0x068*/ int          BuffIDs[NUM_LONG_BUFFS]; // -> 62
/*0x160*/ StatCounter  Statistics[S_LastStat];  // 160
/*0x1f8*/
}; // 1f8

class [[offsetcomments]] MercenaryAbilityInfo
{
public:
/*0x00*/ int Index;
/*0x04*/ int Cost;
/*0x08*/
};

//============================================================================
// CHARINFO a.k.a. PcClient
//============================================================================

class PcClient;
using CHARINFO = PcClient;
using PCHARINFO = PcClient*;

// This type is DEPRECATED. Nothing should be using it. Use CHARINFO or PcClient
using CHARINFONEW DEPRECATE("Use PcClient instead of CHARINFONEW") = PcClient;

inline namespace deprecated
{
	struct BANKARRAY
	{
		ItemClient* Bank[NUM_BANK_SLOTS];
	};

	struct SHAREDBANKARRAY
	{
		ItemClient* SharedBank[NUM_SHAREDBANK_SLOTS];
	};
}

struct [[offsetcomments]] ItemContainingRealEstate
{
	FORCE_SYMBOLS;

/*0x00*/ int                RealEstateID;
/*0x04*/ ItemGlobalIndex    ItemLocation;
/*0x10*/
};

//============================================================================
// FreeToPlayClient
//============================================================================

enum class GameFeature
{
	AA = 0,                  // "AAs"
	MaximumLevel,            // "maximum level"
	CharacterSlots,          // "character slots"
	SpellTier,               // "ability to use this spell tier"
	InventorySlots,          // "inventory slots"
	Platinum,                // "total platinum"
	Mail,                    // "ability to send mail"
	Parcel,                  // "ability to send parcels"
	Loyalty,                 // "ability to spend loyalty"
	MercenaryRank,           // "mercenary ranking cap"
	Housing,                 // "access to a number of guild hall functions"
	SharedBank,              // "shared bank slot access"
	ActiveQuests,            // "number of active quests"
	CreateGuild,             // "ability to create a guild"
	Bazaar,                  // "ability to sell in the bazaar
	Barter,                  // "ability to create buy lines in the bazaar"
	Chat,                    // "ability to use global chat"
	Petition,                // "ability to petition"
	Advertising,             // "advertising mode"
	UseItem,                 // "ability to use this item"
	StartingCity,            // "starting cities"
	Ornament,                // "appearance slot use"
	HeroicCharacter,         // "heroic character"
	AutoGrantAA,             // "auto-grant AAs"
	MountKeyRingSlots,       // "mount key ring slots"
	IllusionKeyRingSlots,    // "illusion key ring slots"
	FamiliarKeyRingSlots,    // "familiar key ring slots"
	FamiliarAutoLeave,       // "familiar auto leave"
	HeroForgeKeyRingSlots,   // "hero's Forge key ring slots"
	DragonHoardSlots,        // "dragon's hoard slots"
	TeleportKeyRingSlots,    // "teleportation item key ring slots"
	PersonalDepotSlots,      // "personal depot slots"

	Max,
	First = 0,
	Invalid = -1,
};

enum DEPRECATE("Use MembershipLevel enumeration instead of eMembershipLevel") eMembershipLevel
{
	MembershipFreeToPlay = 0,
	MembershipSilver,
	MembershipGold
};

enum class MembershipLevel : int8_t
{
	Free = 0,
	Silver,
	AllAccess,
	LifetimeAllAccess,


	Max,
	First = 0,
	Invalid = -1,
	Gold = AllAccess,
};

struct RestrictionInfo
{
	const char* string;
	bool plural;
	bool boolean;
};

class IFreeToPlayInfo
{
public:
	virtual int GetGameFeature(GameFeature feature) const = 0;
	virtual int GetMembershipLevel() const = 0;
};

// size: 0x8c
class [[offsetcomments]] FreeToPlayClient
{
public:
	EQLIB_OBJECT static RestrictionInfo* RestrictionInfo;
	EQLIB_OBJECT static const char* MembershipStrings[(int)MembershipLevel::Max];

	static const char* ToString(GameFeature feature)
	{
		if (feature >= GameFeature::First && feature < GameFeature::Max)
			return RestrictionInfo[(int)feature].string;

		return "Invalid";
	}

	static const char* ToString(MembershipLevel membershipLevel)
	{
		if (membershipLevel >= MembershipLevel::First && membershipLevel < MembershipLevel::Max)
			return MembershipStrings[(int)membershipLevel];

		return "Invalid";
	}

	EQLIB_OBJECT static FreeToPlayClient& Instance();

/*0x00*/ MembershipLevel MembershipLevel;
/*0x04*/ BitField<NUM_RACES> Races;
/*0x08*/ BitField<MAX_PLAYER_CLASSES> Classes;
/*0x0c*/ int LimitData[(int)GameFeature::Max];
/*0x8c*/
};

//============================================================================
// CharacterBase
//============================================================================

enum eCharacterStatus : uint8_t {
	eCharStatusNormal = 0,
	eCharStatusStunned,
	eCharStatusFrozen,
	eCharStatusUnconcious,
	eCharStatusDead,
};

// .rdata:0000000140817C78 const PcClient::`vbtable'{for `CharacterZoneClient'} dd -8
// .rdata:0000000140817C78                                         ; DATA XREF: PcClient::PcClient(void)+2E↑o
// .rdata:0000000140817C7C                 dd 6B8h

// .text:00000001402BCC2E                 lea     rax, const PcClient::`vbtable'{for `CharacterZoneClient'}
// .text:00000001402BCC35                 mov     [rcx+2840h], rax

// this is the offset of CharacterZoneClient + virtual base table:
// CharacterZoneClient starts at 0x2840 and we add 0x6B8 to give us 0x2EF8

// sizeof: 0x20
struct EquipmentItemNode
{
/*0x00*/ uint64_t data1;
/*0x08*/ uint64_t data2;
/*0x20*/
};

class EquipmentItemSet
{
public:
	EquipmentItemSet()
	{
	}

	virtual ~EquipmentItemSet()
	{
	}

	virtual bool IsSwapAllowed() const { return true; }
	virtual uint8_t* Allocate() { return nullptr; }
	virtual void Free(uint8_t*) {}

	struct key_value
	{
		int64_t key;
		int64_t value;
	};

	struct Node
	{
		key_value value;
		Node* parent;
		Node* left;
		Node* right;
	};

	/*0x08*/ Node* root = nullptr;
	/*0x10*/ int count = 0;
};


// size: 0x18
struct EquipmentItem
{
/*0x00*/ int itemId;
/*0x04*/ int slot;
/*0x08*/ EquipmentItemSet items;
/*0x18*/
};

// size: 0x330
struct PersonaEquipmentSet
{
	int classId;
	SoeUtil::EmbeddedArray<EquipmentItem, 23>   Slots;
};

class PersonaEquipmentSetSet
{
public:
	PersonaEquipmentSetSet()
	{
	}

	virtual ~PersonaEquipmentSetSet()
	{
	}

	virtual bool IsSwapAllowed() const { return true; }
	virtual uint8_t* Allocate() { return nullptr; }
	virtual void Free(uint8_t*) {}

	struct key_value
	{
		int key;
		PersonaEquipmentSet value;
	};

	struct Node
	{
		key_value value;
		Node* parent;
		Node* left;
		Node* right;
		uint32_t red : 1;
		uint32_t position : 32;
	};

/*0x08*/ Node* root = nullptr;
/*0x10*/ int count = 0;
};

class [[offsetcomments(0x2ef8)]] CharacterBase : public IFreeToPlayInfo
{
	// +0: vftable
public:
/*0x2f00*/ ProfileManager                        ProfileManager;
/*0x2f50*/ PersonaEquipmentSetSet                PersonaEquipmentSets;
/*0x2f68*/ uint8_t                               languages[MAX_LANGUAGES];
/*0x2f88*/ float                                 X;
/*0x2f8c*/ float                                 Y;
/*0x2f90*/ float                                 Z;
/*0x2f94*/ float                                 Heading;
/*0x2f98*/ char                                  Name[EQ_MAX_NAME];
/*0x2fd8*/ char                                  Lastname[EQ_MAX_LASTNAME];
/*0x2ff8*/ char                                  Title[128];
/*0x3078*/ char                                  VehicleName[64];
/*0x30b8*/ eCharacterStatus                      Status;
/*0x30bc*/ EQZoneIndex                           currentZoneId;
/*0x30c0*/ uint8_t                               standstate;
/*0x30c4*/ RaidData                              raidData;
/*0x31a0*/ int                                   ExpansionFlags;
/*0x31a4*/ bool                                  bSuperPKILL;
/*0x31a5*/ bool                                  bUnclone;
/*0x31a6*/ bool                                  bDead;
/*0x31a8*/ int                                   LD_Timer;
/*0x31ac*/ int                                   SpellInterruptCount;
/*0x31b0*/ bool                                  bAutoSplit;
/*0x31b1*/ bool                                  bTellsOff;
/*0x31b2*/ bool                                  bGmInvis;
/*0x31b4*/ int                                   KillMe;
/*0x31b8*/ bool                                  CheaterLdFlag;                // likely this is int SoulMarkCount instead.
/*0x31b9*/ bool                                  NoRent;
/*0x31ba*/ bool                                  Corpse;
/*0x31bb*/ bool                                  ClientGmFlagSet;
/*0x31bc*/ int                                   BankSharedPlat;
/*0x31c0*/ int                                   BankPlat;
/*0x31c4*/ int                                   BankGold;
/*0x31c8*/ int                                   BankSilver;
/*0x31cc*/ int                                   BankCopper;
/*0x31d0*/ int                                   Platinum;
/*0x31d4*/ int                                   Gold;
/*0x31d8*/ int                                   Silver;
/*0x31dc*/ int                                   Copper;
/*0x31e0*/ int                                   STR;
/*0x31e4*/ int                                   STA;
/*0x31e8*/ int                                   CHA;
/*0x31ec*/ int                                   DEX;
/*0x31f0*/ int                                   INT;
/*0x31f4*/ int                                   AGI;
/*0x31f8*/ int                                   WIS;
/*0x31fc*/ int                                   LCK;
/*0x3200*/ int                                   SavePoison;
/*0x3204*/ int                                   SaveMagic;
/*0x3208*/ int                                   SaveDisease;
/*0x320c*/ int                                   SaveCorruption;
/*0x3210*/ int                                   SaveFire;
/*0x3214*/ int                                   SaveCold;
/*0x3218*/ int                                   SavePhysical;
/*0x321c*/ int                                   UncappedStr;
/*0x3220*/ int                                   UncappedSta;
/*0x3224*/ int                                   UncappedCha;
/*0x3228*/ int                                   UncappedDex;
/*0x322c*/ int                                   UncappedInt;
/*0x3230*/ int                                   UncappedAgi;
/*0x3234*/ int                                   UncappedWis;
/*0x3238*/ int                                   UncappedResistPoison;
/*0x323c*/ int                                   UncappedResistMagic;
/*0x3240*/ int                                   UncappedResistDisease;
/*0x3244*/ int                                   UncappedResistCorruption;
/*0x3248*/ int                                   UncappedResistFire;
/*0x324c*/ int                                   UncappedResistCold;
/*0x3250*/ int                                   UncappedResistPhysical;
/*0x3254*/ int                                   NoBuffStr;
/*0x3258*/ int                                   NoBuffSta;
/*0x325c*/ int                                   NoBuffCha;
/*0x3260*/ int                                   NoBuffDex;
/*0x3264*/ int                                   NoBuffInt;
/*0x3268*/ int                                   NoBuffAgi;
/*0x326c*/ int                                   NoBuffWis;
/*0x3270*/ int                                   NoBuffResistPoison;
/*0x3274*/ int                                   NoBuffResistMagic;
/*0x3278*/ int                                   NoBuffResistDisease;
/*0x327c*/ int                                   NoBuffResistCorruption;
/*0x3280*/ int                                   NoBuffResistFire;
/*0x3284*/ int                                   NoBuffResistCold;
/*0x3288*/ int                                   NoBuffResistPhysical;
/*0x328c*/

	virtual void Copy(const CharacterBase& other) {}
	virtual ~CharacterBase() {}

	ALT_MEMBER_GETTER(int8_t, Status, Stunned);

	uint16_t get_instance() const { return currentZoneId >> 16; }
	__declspec(property(get = get_instance)) uint16_t instance;

	uint16_t get_zoneId() const { return currentZoneId & 0x7fff; }
	__declspec(property(get = get_zoneId)) uint16_t zoneId;

	// Verified
	EQLIB_OBJECT int IsExpansionFlag(int);

	int GetMemorizedSpell(int index) { return GetCurrentBaseProfile().GetMemorizedSpell(index); }
	int GetSpellBook(int index) { return GetCurrentBaseProfile().GetSpellBook(index); }

	// Items
	EQLIB_OBJECT ItemPtr GetItemByGlobalIndex(const ItemGlobalIndex& GlobalIndex) const;
	EQLIB_OBJECT ItemPtr GetItemByGlobalIndex(const ItemGlobalIndex& GlobalIndex, ItemContainer::CheckDepthOptions Option) const;

	//EQLIB_OBJECT bool IsValidGlobalIndex(const ItemGlobalIndex& globalIndex) const;
	//EQLIB_OBJECT /*virtual*/ ItemContainer* GetItemContainerByGlobalIndex(const ItemGlobalIndex& index) const;

	ItemIndex CreateItemIndex(int slot0, int slot1 = -1, int slot2 = -1) const { return GetCurrentBaseProfile().CreateItemIndex(slot0, slot1, slot2); }
	ItemGlobalIndex CreateItemGlobalIndex(int slot0, int slot1 = -1, int slot2 = -1) const { return GetCurrentBaseProfile().CreateItemGlobalIndex(slot0, slot1, slot2); }
	ItemPtr GetItemPossession(const ItemIndex& lIndex) { return GetCurrentBaseProfile().GetItemPossession(lIndex); }
	ItemContainer& GetItemPossessions() { return GetCurrentBaseProfile().GetItemPosessions(); }

	ItemPtr GetInventorySlot(int lIndex) { return GetCurrentBaseProfile().GetInventorySlot(lIndex); }

	BaseProfile& GetCurrentBaseProfile() { return *ProfileManager.GetCurrentProfile(); }
	const BaseProfile& GetCurrentBaseProfile() const { return *ProfileManager.GetCurrentProfile(); }

	int GetPersonaLevel(int classId) { return ProfileManager.GetAltClassLevel(classId - 1); }

	EQLIB_OBJECT ItemIndex FindItemByGuid(const EqItemGuid& ItemGuid);
	EQLIB_OBJECT ItemIndex FindItemById(int ItemId);

	// Fix Typo
	DEPRECATE("Use GetItemPossessions instead of GetItemPosessions")
	ItemContainer& GetItemPosessions() { return GetItemPossessions(); }

	// Accessors
	int GetRace() const { return GetCurrentBaseProfile().GetRace(); }
	int GetClass() const { return GetCurrentBaseProfile().GetClass(); }

	int GetStrength() const { return STR; }
	int GetStamina() const { return STA; }
	int GetCharisma() const { return CHA; }
	int GetDexterity() const { return DEX; }
	int GetIntelligence() const { return INT; }
	int GetAgility() const { return AGI; }
	int GetWisdom() const { return WIS; }
	int GetLuck() const { return LCK; }

	int GetPlatinum() const { return Platinum; }
	int GetGold() const { return Gold; }
	int GetSilver() const { return Silver; }
	int GetCopper() const { return Copper; }
	uint64_t GetTotalCash() const
	{
		return static_cast<uint64_t>(Platinum) * 1000
			+ static_cast<uint64_t>(Gold) * 100
			+ static_cast<uint64_t>(Silver) * 10
			+ Copper;
	}

	EQ_Affect& GetEffect(int nBuffSlot) { return GetCurrentBaseProfile().GetEffect(nBuffSlot); }
	EQ_Affect& GetTempEffect(int nBuffSlot) { return GetCurrentBaseProfile().GetTempEffect(nBuffSlot); }

	int GetEffectSlot(EQ_Affect* effect)
	{
		for (int nBuffSlot = 0; nBuffSlot < MAX_TOTAL_BUFFS; ++nBuffSlot)
		{
			if (effect == &GetEffect(nBuffSlot))
				return nBuffSlot;
		}

		return -1;
	}

	// Unverified
	EQLIB_OBJECT BYTE GetLanguageSkill(int) const;
};

// The starting offset is the size of PcBase
class [[offsetcomments(0x2838)]] CharacterZoneClient : virtual public CharacterBase
{
	virtual void vftableph() {};

	// +0x00: const PcClient::`vftable'{for `CharacterZoneClient'}
	// +0x08: const PcClient::`vbtable'{for `CharacterZoneClient'}

public:
/*0x2848*/ PlayerClient*                         me;
/*0x2850*/ bool                                  statDirtyFlag;
/*0x2851*/ bool                                  zoningStatProcessing;
/*0x2854*/ int                                   ArmorClassBonus;
/*0x2858*/ int                                   CurrWeight;
/*0x285c*/ int                                   LastHitPointSendPercent;
/*0x2860*/ int                                   LastManaPointSendPercent;
/*0x2864*/ int                                   LastEndurancePointSendPercent;
/*0x2868*/ int                                   HPBonus;
/*0x286c*/ int                                   ManaBonus;
/*0x2870*/ int                                   EnduranceBonus;
/*0x2874*/ int                                   EnduranceCostPerSecond;
/*0x2878*/ int                                   CombatEffectsBonus;
/*0x287c*/ int                                   ShieldingBonus;
/*0x2880*/ int                                   SpellShieldBonus;
/*0x2884*/ int                                   AvoidanceBonus;
/*0x2888*/ int                                   AccuracyBonus;
/*0x288c*/ int                                   StunResistBonus;
/*0x2890*/ int                                   StrikeThroughBonus;
/*0x2894*/ int                                   DoTShieldBonus;
/*0x2898*/ int                                   DamageShieldMitigationBonus;
/*0x289c*/ int                                   DamageShieldBonus;
/*0x28a0*/ int                                   ItemSkillMinDamageMod[NUM_ITEM_SKILL_DMG_MOD];
/*0x28c4*/ int                                   SkillMinDamageModBonus[NUM_ITEM_SKILL_DMG_MOD];
/*0x28e8*/ int                                   HeroicSTRBonus;
/*0x28ec*/ int                                   HeroicINTBonus;
/*0x28f0*/ int                                   HeroicWISBonus;
/*0x28f4*/ int                                   HeroicAGIBonus;
/*0x28f8*/ int                                   HeroicDEXBonus;
/*0x28fc*/ int                                   HeroicSTABonus;
/*0x2900*/ int                                   HeroicCHABonus;
/*0x2904*/ int                                   HealAmountBonus;
/*0x2908*/ int                                   SpellDamageBonus;
/*0x290c*/ int                                   ClairvoyanceBonus;
/*0x2910*/ int                                   AttackBonus;
/*0x2914*/ int                                   HPRegenBonus;
/*0x2918*/ int                                   ManaRegenBonus;
/*0x291c*/ int                                   EnduranceRegenBonus;
/*0x2920*/ int                                   AttackSpeed;
/*0x2924*/ int                                   NoBuffItemHitpointAdjustment;
/*0x2928*/ int                                   NoBuffItemManaAdjustment;
/*0x292c*/ int                                   NoBuffItemEnduranceAdjustment;
/*0x2930*/ int                                   NoBuffItemBaseChanceProc;
/*0x2934*/ int                                   NoBuffItemMinDamageMod;
/*0x2938*/ int                                   NoBuffItemInnateSpellRune;
/*0x293c*/ int                                   NoBuffItemAvoidance;
/*0x2940*/ int                                   NoBuffItemToHit;
/*0x2944*/ int                                   NoBuffItemResistStunChance;
/*0x2948*/ int                                   NoBuffItemDotShieldingEffect;
/*0x294c*/ int                                   NoBuffItemStrikeThroughChance;
/*0x2950*/ int                                   NoBuffItemAttack;
/*0x2954*/ int                                   NoBuffItemHitPointRegen;
/*0x2958*/ int                                   NoBuffItemManaRegen;
/*0x295c*/ int                                   NoBuffItemEnduranceRegen;
/*0x2960*/ int                                   NoBuffItemDamageShield;
/*0x2964*/ int                                   NoBuffItemDamageShieldMitigation;
/*0x2968*/ int                                   NoBuffItemHaste;
/*0x296c*/ int                                   NoBuffItemSkillMinDamageMod[NUM_ITEM_SKILL_DMG_MOD];
/*0x2990*/ bool                                  bOutputHpRegen;
/*0x2991*/ bool                                  bInvulnerable;
/*0x2992*/ bool                                  bOnAVehicle;
/*0x2998*/ SpellCache                            spellCache;
/*0x2a28*/ HashListSet<int, 128>                 DoomEffectsBySlot;
/*0x2e48*/ uint32_t                              LastHitEval;
/*0x2e4c*/

	//EQLIB_OBJECT CharacterZoneClient();

	ALT_MEMBER_GETTER(PlayerClient*, me, pSpawn);

	// Verified
	EQLIB_OBJECT /* virtual */ int CalculateInvisLevel(InvisibleTypes Type, bool bIncludeSoS = true);
	EQLIB_OBJECT bool CanUseItem(const ItemPtr& pItem, bool bUseRequiredLvl, bool bOutput = true);
	EQLIB_OBJECT unsigned char CastSpell(unsigned char gemid, int spellid, const ItemPtr& pItem, const ItemGlobalIndex& itemLoc, ItemSpellTypes slot, unsigned char spell_loc, int arg7, int arg8, int arg9, bool arg10);
	EQLIB_OBJECT int Cur_HP(int Spawntype/*PC = 0 NPC=1 and so on*/, bool bCapAtMax = true);
	EQLIB_OBJECT int Cur_Mana(bool bCapAtMax = true);
	EQLIB_OBJECT int GetAdjustedSkill(int);
	EQLIB_OBJECT int GetCurrentMod(int index); // CalculateHeroicModAmount
	/* virtual */ EQLIB_OBJECT int GetBaseSkill(int);
	EQLIB_OBJECT int GetEnduranceRegen(bool bIncItemsAndBuffs = true, bool bCombat = true);
	EQLIB_OBJECT int GetCastingTimeModifier(const EQ_Spell* pSpell); // used to get aa modifiers
	EQLIB_OBJECT int GetFocusCastingTimeModifier(const EQ_Spell* pSpell, ItemPtr& pItemOut, bool bEvalOnly = false);
	EQLIB_OBJECT int GetFocusDurationMod(const EQ_Spell* spell, ItemPtr& pItemOut, PlayerZoneClient* pCaster, int originalDuration, int* pOut1, int* pOut2);
	EQLIB_OBJECT int GetHPRegen(bool bIncItemsAndBuffs = true, bool* pOutIsBleeding = nullptr, bool bCombat = false);
	EQLIB_OBJECT int GetManaRegen(bool bIncItemsAndBuffs = true, bool bCombat = true);
	EQLIB_OBJECT int GetModCap(int index, bool bToggle = false);
	EQLIB_OBJECT EQ_Affect* GetPCSpellAffect(int affectId, int* slotnum, int* spaslot = nullptr) const;
	EQLIB_OBJECT int Max_Endurance(bool bCapAtMax = true);
	EQLIB_OBJECT int Max_HP(int SpawnType, bool bCapAtMax = true);
	EQLIB_OBJECT int Max_Mana(bool bCapAtMax = true);
	EQLIB_OBJECT int SpellDuration(EQ_Spell const*, uint8_t casterLevel, uint8_t item /* bool? */);
	EQLIB_OBJECT int TotalEffect(int spaID, bool bIncludeItems = true, int subindex = 0, bool bIncludeAA = true, bool bIncludeBuffs = true) const;
	EQLIB_OBJECT void UseSkill(unsigned char skill, PlayerZoneClient* Target, bool bAuto = false);

	EQLIB_OBJECT int GetFocusReuseMod(const EQ_Spell* pSpell, ItemPtr& pOutItem, bool evalOnly = false);
	EQLIB_OBJECT int GetFocusRangeModifier(const EQ_Spell* pSpell, ItemPtr& pItemOut);

	EQLIB_OBJECT void GetPctModAndMin(const EQ_Spell* pSpell, int spa, ItemPtr& pOutItem, int& outPctMod, int& outMin, bool wMins, bool evalOnly = false, bool all = false, CharacterZoneClient* caster = nullptr);

	// Unverified

	EQLIB_OBJECT int CalcAffectChange(const EQ_Spell* spell, BYTE casterLevel, BYTE affextIndex, const EQ_Affect* theAffect, int EffectIndex = 0, PlayerZoneClient* pCaster = NULL, bool overrideChangeVal = false, int ChangeVal = -1, bool bCap = true);
	EQLIB_OBJECT int CalcAffectChangeGeneric(const EQ_Spell* spell, BYTE casterLevel, BYTE affextIndex, const EQ_Affect* theAffect, int EffectIndex, bool bCap = true);
	EQLIB_OBJECT void MakeMeVisible(int, bool);
	EQLIB_OBJECT int GetItemCountWorn(int);
	EQLIB_OBJECT int GetItemCountInInventory(int, bool skipCheck = false);
	EQLIB_OBJECT int GetCursorItemCount(int);
	EQLIB_OBJECT bool HasSkill(int);
	EQLIB_OBJECT EQ_Affect* FindAffectSlot(int SpellID, PlayerClient* Caster, int* slindex, bool bJustTest, int CasterLevel = -1, EQ_Affect* BuffArray = nullptr, int BuffArraySize = 0);
	EQLIB_OBJECT bool IsStackBlocked(const EQ_Spell* pSpell, PlayerClient* pCaster, EQ_Affect* pEffecs = NULL, int EffectsSize = 0, bool bMessageOn = false);
	EQLIB_OBJECT int BardCastBard(const EQ_Spell* pSpell, signed int caster_class) const;
	EQLIB_OBJECT unsigned char GetMaxEffects() const;
	EQLIB_OBJECT int GetOpenEffectSlot(bool bIsShortBuff, bool bIsMeleeSkill, int Index = -1);
	EQLIB_OBJECT int GetFirstEffectSlot(bool bIsShortBuff, bool bIsMeleeSkill);
	EQLIB_OBJECT int GetLastEffectSlot(bool bIsShortBuff, bool bIsMeleeSkill, bool bIsDisplay = false);

	// From EQ_Character1
	// int const GetFocusCastingTimeModifier(class EQ_Spell const*, class EQ_Equipment**, int);
	EQLIB_OBJECT void SetEffectId(unsigned char, unsigned int);
	EQLIB_OBJECT void StopSpellCast(unsigned char);
	EQLIB_OBJECT void StopSpellCast(unsigned char, int);
	EQLIB_OBJECT void StunMe(unsigned int, bool, bool, bool);
	EQLIB_OBJECT void UnStunMe();

	// From EQ_Character
	EQLIB_OBJECT bool DoesSpellMatchFocusFilters(EQ_Spell const*, EQ_Spell const*);
	EQLIB_OBJECT bool IsSpellTooPowerfull(EQ_Spell*, CharacterZoneClient*);
	EQLIB_OBJECT bool CanUseMemorizedSpellSlot(int gem);
	EQLIB_OBJECT bool IsValidAffect(int);
	EQLIB_OBJECT char* Class(int);
	EQLIB_OBJECT char* KunarkClass(int, int, int, bool);
	EQLIB_OBJECT char* Race(int);
	//EQLIB_OBJECT EQ_Affect& GetEffect(int) const; // removeme
	EQLIB_OBJECT EQ_Equipment* GetFocusItem(EQ_Spell const*, int);
	EQLIB_OBJECT EQ_Spell* GetFocusEffect(EQ_Spell const*, int);
	EQLIB_OBJECT PlayerClient* FindClosest(int, int, int, int, int);
	EQLIB_OBJECT PlayerClient* GetMyPetPlayer();
	EQLIB_OBJECT float encum_factor();
	EQLIB_OBJECT int ac(bool);
	EQLIB_OBJECT int Agi();
	EQLIB_OBJECT int AntiTwinkAdj(EQ_Equipment*, int, int);
	EQLIB_OBJECT int ApplyDamage(int, EQMissileHitinfo*, bool, HateListEntry*, bool);
	EQLIB_OBJECT int ApplyFatigue(int);
	EQLIB_OBJECT int AutoEat(unsigned char);
	EQLIB_OBJECT int BaneDamage(EQ_Equipment*);
	//EQLIB_OBJECT int BardCastBard(EQ_Spell const*, int) const;
	EQLIB_OBJECT int basesave_cold();
	EQLIB_OBJECT int basesave_disease();
	EQLIB_OBJECT int basesave_fire();
	EQLIB_OBJECT int basesave_magic();
	EQLIB_OBJECT int basesave_poison();
	EQLIB_OBJECT int CalculateBardSongMod(int);
	EQLIB_OBJECT int CapStat(int, int);
	EQLIB_OBJECT int Cha();
	EQLIB_OBJECT int CheckFoodAndWater();
	EQLIB_OBJECT int compute_defense();
	EQLIB_OBJECT int compute_tohit(unsigned char);
	EQLIB_OBJECT int cur_encumbrance();
	EQLIB_OBJECT int defense_agility_bonus();
	EQLIB_OBJECT int Dex();
	EQLIB_OBJECT int ElementResistDmg(EQ_Equipment*, int);
	EQLIB_OBJECT int eqspa_change_form(EQ_Spell*, int, int, EQ_Affect*);
	EQLIB_OBJECT int eqspa_hp(unsigned int, EQ_Affect*, int, EQ_Spell*);
	EQLIB_OBJECT int GetBardInstrumentMod(int);
	EQLIB_OBJECT int GetClassACBonus(int);
	EQLIB_OBJECT int GetFirstEffectSlot(bool);
	EQLIB_OBJECT int GetHPFromStamina(int);
	EQLIB_OBJECT int GetIndexSkillMinDamageMod(int);
	EQLIB_OBJECT int GetLastEffectSlot(bool);
	EQLIB_OBJECT int GetOpenEffectSlot(bool, int);
	EQLIB_OBJECT int HasInnateSkill(unsigned char, int);
	EQLIB_OBJECT int HasSkill(unsigned char);
	EQLIB_OBJECT int HasSpell(int);
	EQLIB_OBJECT int IHaveSkill(unsigned char);
	EQLIB_OBJECT int InSpecialBindSpot(PlayerZoneClient*);
	EQLIB_OBJECT int Int();
	EQLIB_OBJECT int IsMage();
	EQLIB_OBJECT int IsSpecialBazaarSpot(PlayerZoneClient*);
	EQLIB_OBJECT int IsSpellcaster();
	EQLIB_OBJECT int ItemSpellAffects(int);
	EQLIB_OBJECT int max_encumbrance();
	EQLIB_OBJECT int NoBashMe(int);
	EQLIB_OBJECT int NoMezMe(int, PlayerZoneClient*, EQ_Spell*);
	EQLIB_OBJECT int offense(unsigned char);
	EQLIB_OBJECT int ProcessAffects();
	EQLIB_OBJECT int save_bash(int);
	EQLIB_OBJECT int save_cold();
	EQLIB_OBJECT int save_disease();
	EQLIB_OBJECT int save_fire();
	EQLIB_OBJECT int save_magic();
	EQLIB_OBJECT int save_poison();
	EQLIB_OBJECT int SetLocalVar(char*, int);
	EQLIB_OBJECT int Skill(int);
	EQLIB_OBJECT int Sta();
	EQLIB_OBJECT int Str();
	EQLIB_OBJECT int TakeFallDamage(float);
	EQLIB_OBJECT int TotalSpellAffects(unsigned char, bool, int*);
	EQLIB_OBJECT int Wis();
	EQLIB_OBJECT int const GetFocusConserveRegChance(const EQ_Spell*, EQ_Equipment**);
	EQLIB_OBJECT long TotalOnPerson();
	EQLIB_OBJECT unsigned char BarbarianCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char BaseAttr(int, unsigned char);
	EQLIB_OBJECT unsigned char CanDoubleAttack(PlayerZoneClient*, unsigned char);
	EQLIB_OBJECT unsigned char CanIBreathe();
	EQLIB_OBJECT unsigned char CanISeeInvis();
	EQLIB_OBJECT unsigned char CanMedOnHorse();
	EQLIB_OBJECT unsigned char CanSecondaryAttack(PlayerZoneClient*);
	EQLIB_OBJECT unsigned char CanWorship(int, int, unsigned char);
	EQLIB_OBJECT unsigned char CastingRequirementsMet(int);
	EQLIB_OBJECT unsigned char CheckClass(int, int);
	EQLIB_OBJECT unsigned char CityCanStart(int, int, int, int);
	EQLIB_OBJECT unsigned char ClassMin(int, int, unsigned char);
	EQLIB_OBJECT unsigned char DarkElfCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char DwarfCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char ElfCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char EruditeCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char ExpendItemCharge(int, int);
	EQLIB_OBJECT unsigned char FindItemByClass(int, int*, int*);
	EQLIB_OBJECT unsigned char FindItemQty(int, int);
	EQLIB_OBJECT unsigned char FroglockCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char GetSkillBaseDamage(unsigned char, PlayerZoneClient*);
	EQLIB_OBJECT unsigned char GnomeCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char HalfElfCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char HalflingCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char HighElfCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char HumanCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char IksarCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char IsSpellAffectingPC(int, int);
	EQLIB_OBJECT unsigned char OgreCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char SpellFizzled(unsigned char, EQ_Spell*);
	EQLIB_OBJECT unsigned char TrollCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT unsigned char VahShirCanWorship(unsigned char, unsigned char);
	EQLIB_OBJECT void CalcFoodDrinkBonus(int);
	EQLIB_OBJECT void DoFishingEvent();
	EQLIB_OBJECT void DoIntimidationEvent();
	EQLIB_OBJECT void DoLight(unsigned char);
	EQLIB_OBJECT void DoMeditation();
	EQLIB_OBJECT void DoPassageOfTime();
	EQLIB_OBJECT void EQ_CharacterResetAllMembers();
	EQLIB_OBJECT void EQSPA_Feign_Death(int);
	EQLIB_OBJECT void eqspa_levitation();
	EQLIB_OBJECT void eqspa_locate_corpse();
	EQLIB_OBJECT void eqspa_movement_rate(int);
	EQLIB_OBJECT void HandleSpecialPCAffects(int);
	EQLIB_OBJECT void HitBySpell(EQMissileHitinfo*);
	EQLIB_OBJECT void IAmDead(EQMissileHitinfo*, int);
	EQLIB_OBJECT void InitInnates(unsigned int, unsigned int);
	EQLIB_OBJECT void InitMyLanguages();
	EQLIB_OBJECT void InitSkills(unsigned char, unsigned int);
	EQLIB_OBJECT void ItemSold(long);
	EQLIB_OBJECT void ModifyCurHP(int64_t modification, PlayerZoneClient* resposibleplayer, int skilltype);
	EQLIB_OBJECT void NotifyPCAffectChange(int, int);
	EQLIB_OBJECT void ProcessAllStats();
	EQLIB_OBJECT void ProcessEnvironment();
	EQLIB_OBJECT void ProcessHungerandThirst(unsigned int);
	EQLIB_OBJECT void RemovePCAffect(EQ_Affect*);
	EQLIB_OBJECT void RemovePCAffectex(EQ_Affect*, bool, ArrayClass<LaunchSpellData*>&, int, int, int);
	EQLIB_OBJECT void ResetCur_HP(int);
	EQLIB_OBJECT void UpdateMyVisibleStatus();

	// private from EQ_Character
	EQLIB_OBJECT int GetCachEQSPA(int);
	EQLIB_OBJECT void ReCachItemEffects();
	EQLIB_OBJECT void ReCachSpellEffects();

	// Status Bonuses
	int GetDamageShieldBonus() const { return DamageShieldBonus; }
};

using MailItemsHash = HashTable<MailItemData, EqItemGuid, ResizePolicyNoShrink>;
using AchievementTable = HashTable<CompletedAchievementData, int, ResizePolicyNoShrink>;
using AchievementSubComponentCountTable = HashTable<AchievementSubComponentCountData, int, ResizePolicyNoShrink>;

class [[offsetcomments]] PcBase : virtual public CharacterBase
{
public:
/*0x0008*/ // void*    vfTable_CharacterZoneClient;
/*0x0008*/ // void*    vfTable_CharacterBase;
/*0x000C*/ // void*    vfTable_PcBase;
// last one changed
/*0x000C*/ virtual void vftableph() {};

/*0x0010*/ int                                   RecentTasks[10];
/*0x0038*/ PCTaskStatus                          Tasks[1];
/*0x00a8*/ PCTaskStatus                          Quests[29];
/*0x0d58*/ uint8_t                               BitFlags[6400 / 8];
/*0x1078*/ BenefitSelection                      ActiveTributeBenefits[5];
/*0x10a0*/ BenefitSelection                      ActiveTrophyTributeBenefits[10];
/*0x10f0*/ uint8_t                               BitFlags2[6400 / 8];
/*0x1410*/ ItemContainer                         BankItems;
/*0x1438*/ ItemContainer                         SharedBankItems;
/*0x1460*/ ItemContainer                         OverflowBufferItems;
/*0x1488*/ ItemContainer                         LimboBufferItems;
/*0x14b0*/ ItemContainer                         MercenaryItems;
/*0x14d8*/ ItemContainer                         MountKeyRingItems;
/*0x1500*/ ItemContainer                         IllusionKeyRingItems;
/*0x1528*/ ItemContainer                         FamiliarKeyRingItems;
/*0x1550*/ ItemContainer                         HeroForgeKeyRingItems;
/*0x1578*/ ItemContainer                         TeleportationKeyRingItems;
/*0x15a0*/ ItemContainer                         DragonHoardItems;
/*0x15c8*/ ItemContainer                         PersonalTradeskillDepotItems;
/*0x15f0*/ ItemContainer                         AltStorageItems;
/*0x1618*/ ItemContainer                         ArchivedDeletedItems;
/*0x1640*/ ItemContainer                         MailItems;
/*0x1668*/ MailItemsHash                         MailItemsData;
/*0x1680*/ eqtime_t                              RecentMoves[1];
/*0x1688*/ HashTable<DynamicZoneData>            CurrentDynamicZones;
/*0x16a0*/ HashTable<int>                        LearnedRecipes;
/*0x16b8*/ EQList<TradeskillRecipeCount*>        QualifyingRecipeCounts;
/*0x16d8*/ HashTable<int>                        NonrepeatableQuests;
/*0x16f0*/ HashTable<int>                        CompletedTasks;
/*0x1708*/ HashTable<int>                        CompletedTasks2;
/*0x1720*/ eqtime_t                              AlchemyTimestamp;
/*0x1728*/ bool                                  bWhat;
/*0x1729*/ bool                                  bSomethingHome;
/*0x172c*/ uint32_t                              LoginTime;
/*0x1730*/ EqGuid                                GuildID;
/*0x1738*/ EqGuid                                FellowshipID;
/*0x1740*/ SFellowship*                          pFellowship;
/*0x1748*/ bool                                  GuildShowSprite;
/*0x1750*/ eqtime_t                              CreationTime;
/*0x1758*/ eqtime_t                              AccountCreationTime;
/*0x1760*/ eqtime_t                              LastPlayedTime;
/*0x1768*/ uint32_t                              MinutesPlayed;
/*0x176c*/ uint8_t                               Anonymous;
/*0x176d*/ bool                                  bGM;
/*0x176e*/ bool                                  bGMStealth;
/*0x1770*/ int                                   AirSupply;
/*0x1774*/ uint32_t                              SerialNum;
/*0x1778*/ bool                                  bNewCharacter;
/*0x177c*/ int                                   TasksAssigned;
/*0x1780*/ int                                   TasksCompleted;
/*0x1784*/ long                                  TaskRequestTimer;
/*0x1788*/ uint32_t                              UniquePlayerID;
/*0x178c*/ WorldLocation                         DynamicZoneSafeReturnLocation;
/*0x17a0*/ DynamicZoneTimerData*                 pDZTimerRoot;
/*0x17a8*/ int                                   BenefitTimer;
/*0x17ac*/ int                                   TrophyBenefitTimer;
/*0x17b0*/ int64_t                               CareerFavor;
/*0x17b8*/ int64_t                               CurrFavor;
/*0x17c0*/ bool                                  bBenefitsActive;
/*0x17c1*/ bool                                  bTrophyBenefitsActive;
/*0x17c2*/ bool                                  bHasResetStartingCity;
/*0x17c4*/ int                                   PvPKills;
/*0x17c8*/ int                                   PvPDeaths;
/*0x17cc*/ int                                   PvPCurrentPoints;
/*0x17d0*/ int                                   PvPTotalPointsEarned;
/*0x17d4*/ int                                   PvPKillStreak;
/*0x17d8*/ int                                   PvPDeathStreak;
/*0x17dc*/ int                                   PvPCurrentStreak;
/*0x17e0*/ PvPKill                               LastKill;
/*0x1840*/ PvPDeath                              LastDeath;
/*0x1898*/ HashTable<PvPKill24HourData>          PvPLast24HoursKillHash;
/*0x18b0*/ int                                   PvPInfamyLevel;
/*0x18b4*/ int                                   PvPVitality;
/*0x18b8*/ eqtime_t                              PvPLastInfamyTime;
/*0x18c0*/ eqtime_t                              LastLastNameChange;
/*0x18c8*/ int                                   LastNameChangePriv;
/*0x18cc*/ uint32_t                              PvPLastVitalityTime;
/*0x18d0*/ int                                   GoodPointsAvailable;         // a.k.a. RadiantCrystals
/*0x18d4*/ int                                   GoodTotalPointsEarned;
/*0x18d8*/ int                                   EvilPointsAvailable;         // a.k.a. EbonCrystals
/*0x18dc*/ int                                   EvilTotalPointsEarned;
/*0x18e0*/ bool                                  bCanRequestNameChange;
/*0x18e1*/ bool                                  bCanRequestNameChange2;
/*0x18e2*/ bool                                  bCanRequestServerTransfer;
/*0x18e3*/ bool                                  bIsCopied;
/*0x18e8*/ eqtime_t                              ServerTransferGrantTime;
/*0x18f0*/ bool                                  bCanRequestRaceChange;
/*0x18f4*/ uint32_t                              LastAAResetTime;
/*0x18f8*/ uint32_t                              LastMercAAResetTime;
/*0x18fc*/ EQZoneIndex                           NewZoneID;
/*0x1900*/ int                                   NewAreaID;
/*0x1904*/ EAreaCorner                           eNewAreaCorner;
/*0x1908*/ EQZoneIndex                           PreviousZoneID;
/*0x190c*/ int                                   RealEstateZoneID;
/*0x1910*/ char                                  ServerCreated[32];
/*0x1930*/ PCAdventureData                       AdventureData;
/*0x1a28*/ PCSharedTaskData                      SharedTaskData;
/*0x1ac0*/ TaskTimerData*                        pTaskTimerData;
/*0x1ac8*/ PCQuestHistoryData                    QuestHistoryData;
/*0x1de8*/ PCStatistics                          PcStatistics;
/*0x1f50*/ GroupMemberStats                      GroupStats;
/*0x2148*/ bool                                  bIsLfg; // 2148
/*0x214c*/ int                                   RaidId;
/*0x2150*/ int                                   GroupID;
/*0x2154*/ int                                   DaysEntitled;
/*0x2158*/ bool                                  bAutoConsentGroup;
/*0x2159*/ bool                                  bAutoConsentRaid;
/*0x215a*/ bool                                  bAutoConsentGuild;
/*0x215b*/ bool                                  bAutoConsentFellowship;
/*0x215c*/ bool                                  bPrivateForEqPlayers;
/*0x2160*/ long                                  AchievementFilesModificationTime;
/*0x2164*/ char                                  StationID[EQ_MAX_STATION_ID];
/*0x2188*/ EqGuid                                Guid;
/*0x2190*/ bool                                  bBetaBuffed;
/*0x2194*/ int                                   Unknown0x1ee4;
/*0x2198*/ int                                   MainLevel;
/*0x219c*/ bool                                  bShowHelm;
/*0x21a0*/ int64_t                               LastTestCopyTime;
/*0x21a8*/ CPlayerPointManager                   PointManager;
/*0x21c8*/ PointSystemBase                       PointSystem;
/*0x21f8*/ uint32_t                              LoyaltyVelocity;
/*0x21fc*/ uint32_t                              LoyaltyTokens;
/*0x2200*/ bool                                  bHasLoyaltyInfo;
/*0x2208*/ ArrayClass<int>                       OwnedRealEstates;
/*0x2220*/ ArrayClass<int>                       OwnedItemRealEstates;
/*0x2238*/ ArrayClass<int>                       ArchivedRealEstates;
/*0x2250*/ char                                  OverridePetName[EQ_MAX_NAME];
/*0x2290*/ bool                                  bCanRequestPetNameChange;
/*0x2291*/ char                                  OverrideFamiliarName[EQ_MAX_NAME];
/*0x22d1*/ bool                                  bCanRequestFamiliarNameChange;
/*0x22d8*/ CXStr                                 OverrideMercName[EQ_MAX_MERCENARIES];
/*0x2330*/ bool                                  bCanRequestMercNameChange;
/*0x2334*/ uint32_t                              LegacyExperienceBonus;
/*0x2338*/ PendingRewardList                     PendingRewards;
/*0x2380*/ uint8_t                               WaitingRewards[0x20]; // type is unknown
/*0x23a0*/ uint32_t                              DowntimeReductionTime;
/*0x23a8*/ uint64_t                              DowntimeTimerStart;
/*0x23b0*/ float                                 ActivityValue;
/*0x23b4*/ uint32_t                              NextItemId;
/*0x23b8*/ CXStr                                 SharedBank;                      // SharedVaultInstanceData
/*0x23c0*/ CXStr                                 BankBuffer;                      // BankInstanceData
/*0x23c8*/ CXStr                                 LimboBuffer;                     // LimboInstanceData
/*0x23d0*/ CXStr                                 MercenaryBuffer;                 // MercenaryInstanceData
/*0x23d8*/ CXStr                                 KeyRingBuffer[eKeyRingTypeCount];// MountKeyRingInstanceData
/*0x2400*/ CXStr                                 AltStorageBuffer;                // AltStorageInstanceData
/*0x2408*/ CXStr                                 OverflowBuffer;                  // ItemOverflow
/*0x2410*/ CXStr                                 DragonHoardBuffer;               // DragonHoardInstanceData
/*0x2418*/ SoeUtil::String                       EquipmentSetsBuffer;             // EquipmentSetsData
/*0x2430*/ CXStr                                 TradeskillDepotBuffer;           // PersonalTradeskillDepotInstanceData
/*0x2438*/ bool                                  DragonHoardPopulated;
/*0x243c*/ uint32_t                              DragonHoardCapacity;
/*0x2440*/ uint32_t                              TradeskillDepotCapacity;
/*0x2444*/ uint32_t                              TradeskillDepotPopulated;
/*0x2448*/ uint32_t                              AltStorageTimestamp;             // AltStorageTimestamp
/*0x244c*/ ELockoutCharacterReason               LockoutCharacterReason;
/*0x2450*/ HashTable<ProgressionExperience>      ProgressionExp;
/*0x2468*/ CXStr                                 ArchivedStorageBuffer;           // ArchiveStorageInstanceData
/*0x2470*/ CXStr                                 MailItemsBuffer;
/*0x2478*/ CXStr                                 MailItemsDataBuffer;
/*0x2480*/ int                                   MailItemsOverCapWarningCount;
/*0x2484*/ bool                                  UseAdvancedLooting;
/*0x2485*/ bool                                  MasterLootCandidate;
/*0x2486*/ bool                                  bIsCorrupted; // -40a
/*0x2488*/ char*                                 pCorruptionReport;
/*0x2490*/ char                                  InspectText[256];
/*0x2590*/ HashTable<int>                        BlockedSpellsHash;
/*0x25a8*/ int                                   BlockedSpell[MAX_BLOCKED_SPELLS];
/*0x2698*/ HashTable<int>                        BlockedPetSpellsHash;
/*0x26b0*/ int                                   BlockedPetSpell[MAX_BLOCKED_SPELLS_PET];
/*0x27a0*/ ClaimDataCollection                   ConsumableFeatures;
/*0x27b8*/ bool                                  bGrantItemsRegistered;
/*0x27c0*/ uint64_t                              CreatedGuildID;
/*0x27c8*/ eqtime_t                              GuildCreateTime;
/*0x27d0*/ CXStr                                 GuildCreateCharacter;
/*0x27d8*/ bool                                  bInventoryUnserialized;
/*0x27d9*/ bool                                  bAltStorageUnserialized;
/*0x27da*/ bool                                  bArchivedStorageUnserialized;
/*0x27db*/ bool                                  bMailUnserialized;
/*0x27dc*/ bool                                  bPendingInventorySerialization;
/*0x27e0*/ CXStr                                 BuyLines;
/*0x27e8*/ ArrayClass<CXStr>                     OfflineTraderSoldItems;
/*0x2800*/ ArrayClass<CXStr>                     OfflineBuyerBoughtItems;
/*0x2818*/ uint32_t                              Krono;
/*0x281c*/ uint32_t                              CursorKrono;
/*0x2820*/ int64_t                               MercAAExp;                    // divide this with 3.30f and you get the percent
/*0x2828*/ int                                   MercAAPoints;                 // number of unspent merc AA points
/*0x282c*/ int                                   MercAAPointsSpent;            // number of spent merc AA points
/*0x2830*/ ArrayClass<MercenaryAbilityInfo*>     MercenaryAbilities;
/*0x2848*/ AchievementTable                      CompletedAchievements;
/*0x2860*/ AchievementSubComponentCountTable     CompletedEventBasedSubComponents;
/*0x2878*/ AchievementSubComponentCountTable     OpenEventBasedSubComponents;
/*0x2890*/ int                                   LastFellowshipJoin;
/*0x2898*/ int64_t                               Vitality;
/*0x28a0*/ int64_t                               AAVitality;
/*0x28a8*/ int                                   FreeToPlayUnlocks[33];
/*0x292c*/ // end PcBase / start CharacterZoneClient

	// basis: 0x2848

	PcProfile* GetCurrentPcProfile() { return (PcProfile*)&GetCurrentBaseProfile(); }
	PcProfile* GetCurrentPcProfile() const { return (PcProfile*)&GetCurrentBaseProfile(); }

	EQLIB_OBJECT ItemContainer& GetKeyRingItems(KeyRingType type);
	ItemPtr GetKeyRingItem(KeyRingType type, int index) { return GetKeyRingItems(type).GetItem(index); }
	ItemPtr GetKeyRingItem(KeyRingType type, const ItemIndex& index) { return GetKeyRingItems(type).GetItem(index); }
	const ItemIndex& GetStatKeyRingItemIndex(KeyRingType type) const { return GetCurrentBaseProfile().StatKeyRingItemIndex[type]; }
	EQLIB_OBJECT ItemIndex FindKeyRingItemByGuid(KeyRingType type, const EqItemGuid& guid);
	EQLIB_OBJECT ItemIndex FindKeyRingItemById(KeyRingType type, int itemId);

	bool IsFamiliarAutoLeaveEnabled() const { return GetGameFeature(GameFeature::FamiliarAutoLeave) != 0; }

	int GetCombatAbility(int index) const { return GetCurrentPcProfile()->GetCombatAbility(index); }

	// Stores information about purchased Mercenary Abilities
	EQLIB_OBJECT const MercenaryAbilityInfo* GetMercenaryAbilityInfo(int abilityId) const;
	float GetMercAAExpPct() const { return (MercAAExp + 5) / 10.f; }
	int64_t GetMercAAExp() const { return MercAAExp; }
	int GetMercAAPoints() const { return MercAAPoints; }
	int GetMercAAPointsSpent() const { return MercAAPointsSpent; }

	int GetAirSupply() const { return AirSupply; }
	int GetLevel() const { return GetCurrentPcProfile()->Level; }
	int GetDeity() const { return GetCurrentPcProfile()->Deity; }

	int GetTradeskillDepotCapacity() const { return TradeskillDepotCapacity; }
	bool GetTradeskillDepotPopulated() const { return TradeskillDepotPopulated != 0; }

	ALT_MEMBER_GETTER(int, GoodPointsAvailable, RadiantCrystals);
	ALT_MEMBER_GETTER(int, EvilPointsAvailable, EbonCrystals);
	ALT_MEMBER_GETTER(eqtime_t, CreationTime, CharCreationTime);
	ALT_MEMBER_GETTER(int, BenefitTimer, TributeTimer);

	//----------------------------------------------------------------------------
	// These properties moved to the PcProfile
	int GetStartingCity() const { return GetCurrentBaseProfile().StartingCity; }
	__declspec(property(get = GetStartingCity))
		DEPRECATE("Use GetStartingCity instead of accessing StartingCity directly")
		int StartingCity;

	uint32_t GetAlternateAbilityExp() const { return GetCurrentBaseProfile().AAExp; }
	__declspec(property(get = GetAlternateAbilityExp))
		uint32_t AAExp;

	uint8_t GetPercentExpToAA() const { return GetCurrentBaseProfile().PercentEXPtoAA; }
	__declspec(property(get = GetPercentExpToAA))
		uint8_t PercentEXPtoAA;

	int64_t GetExperience() const { return GetCurrentBaseProfile().Exp; }
	__declspec(property(get = GetExperience))
		int64_t Exp;
};

class DebugText
{
public:
	EQLIB_OBJECT virtual void SendDebugText(const char* text, int color);
};

class [[offsetcomments]] PcZoneClient : public PcBase, public CharacterZoneClient, public DebugText
{
/*0x285c*/ // vftable 2e58 -> 2f48
public:
/*0x2f50*/ uint32_t                              Flags[3];  // 2f50
/*0x2f5c*/ uint32_t                              TransfersReceived;
/*0x2f60*/ int                                   LastLanguageSpoken;
/*0x2f64*/ int                                   CurPowerSourceDrain;
/*0x2f68*/ EQList<ALCHEMYBONUSSKILLDATA*>        AlchemyBaseSkillBonusList; // 2f68
/*0x2f88*/ uint32_t                              MomentumBalance;
/*0x2f8c*/ uint32_t                              LoyaltyRewardBalance;
/*0x2f90*/ // 2f90

	// Verified
	EQLIB_OBJECT void DestroyHeldItemOrMoney();
	EQLIB_OBJECT bool DoCombatAbility(int spellID, bool allowLowerRank = true);
	EQLIB_OBJECT int GetPcSkillLimit(int, bool = true);
	EQLIB_OBJECT void RemovePetEffect(int);
	EQLIB_OBJECT bool HasAlternateAbility(int aaindex, int* pIndex = nullptr, bool bProfile = false, bool bMerc = false);
	EQLIB_OBJECT bool CanEquipItem(const ItemPtr& pItem, int slotid, bool bOutputDebug, bool bUseRequiredLevel = false);
	// If allSlots is false, only checks slots the user has perks for.
	EQLIB_OBJECT ItemPtr GetItemByID(int itemid, ItemIndex* itemindex = nullptr, bool allSlots = false);
	EQLIB_OBJECT ItemPtr GetItemByItemClass(int itemclass, ItemIndex* itemindex = nullptr);
	EQLIB_OBJECT void RemoveBuffEffect(int Index, int SpawnID);
	EQLIB_OBJECT void BandolierSwap(int index);
	uint32_t GetLinkedSpellReuseTimer(int index) { return GetCurrentPcProfile()->GetLinkedSpellReuseTimer(index); }
	EQLIB_OBJECT uint32_t GetItemRecastTimer(const ItemPtr& item, ItemSpellTypes etype);

	// Unverified
	EQLIB_OBJECT bool HasCombatAbility(int);

	EQLIB_OBJECT int CanUseMeleeCombatAbility(int SpellID) const;

	EQLIB_OBJECT int GetMaxAirSupply() const;

	EQLIB_OBJECT static int GetDeityReal(int diety);
	int GetDeityReal() const { return GetDeityReal(GetDeity()); }    // Returns values from EQDeity
	int GetDeityBitmask() const { return 1 << (GetDeityReal() - 1); }
};

// @sizeof(PcClient) == 0x3388 :: 2024-06-17 (live) @ 0x14027232b
constexpr size_t PcClient_size = 0x3388;

class [[offsetcomments]] PcClient : public PcZoneClient
{
	// has a vftable but we get it from PcZoneClient
public:
	EQLIB_OBJECT PcClient();

/*0x2f90*/ ExtendedTargetList*                   pExtendedTargetList;          // cannot be null  // 2f90
/*0x2f98*/ bool                                  InCombat;
/*0x2f9c*/ uint32_t                              Downtime;
/*0x2fa0*/ uint32_t                              DowntimeStart;
/*0x2fa4*/ bool                                  bOverrideAvatarProximity;
/*0x2fa8*/ CGroup*                               Group;
/*0x2fb0*/ bool                                  bIAmCreatingGroup;
/*0x2fb8*/ ItemArray                             ItemsPendingID;
/*0x2fd0*/ eParcelStatus                         ParcelStatus;
/*0x2fd4*/ int                                   SubscriptionDays;
/*0x2fd8*/ short                                 BaseKeyRingSlots[eKeyRingTypeCount];
/*0x2fe2*/ bool                                  bPickZoneFewest;
/*0x2fe4*/ int                                   Unknown0x28a4;                // used in CContainerWnd::HandleCombine
/*0x2fe8*/

	ALT_MEMBER_GETTER(ExtendedTargetList*, pExtendedTargetList, pXTargetMgr);  // cannot be null
	ALT_MEMBER_GETTER(uint32_t, DowntimeStart, DowntimeStamp);
	ALT_MEMBER_GETTER(CGroup*, Group, pGroupInfo);

	// Verified
	EQLIB_OBJECT unsigned long GetConLevel(const PlayerClient*);
	EQLIB_OBJECT bool HasLoreItem(const ItemPtr&, bool, bool, bool, bool);

	virtual int GetGameFeature(GameFeature) const override { return 0; }
	virtual int GetMembershipLevel() const override { return 0; }

	// Unverified
	// TODO: Methods from EQ_PC: The ones we use need to be validated. Not all of them live in PcClient.
	EQLIB_OBJECT int CheckDupLoreItems();
	EQLIB_OBJECT int checkLang(int);
	EQLIB_OBJECT int CostToTrain(int, float, int);
	EQLIB_OBJECT int GetAlternateAbilityId(int);
	EQLIB_OBJECT int GetArmorType(int);
	EQLIB_OBJECT PcZoneClient* GetPcZoneClient() const;
	EQLIB_OBJECT int HandleMoney(long);
	EQLIB_OBJECT int IsAGroupMember(char*);
	EQLIB_OBJECT int MaxAirSupply();
	EQLIB_OBJECT int numInParty();
	EQLIB_OBJECT unsigned char AtSkillLimit(int);
	EQLIB_OBJECT unsigned char RemoveMyAffect(int);
	EQLIB_OBJECT unsigned long GetArmorTint(int);
	EQLIB_OBJECT unsigned long GetBodyTint(int);
	EQLIB_OBJECT void CheckForGroupChanges();
	EQLIB_OBJECT void CheckForLanguageImprovement(unsigned char, unsigned char);
	EQLIB_OBJECT void CheckSkillImprove(int, float);
	EQLIB_OBJECT void ClearSharedVault();
	EQLIB_OBJECT void DetermineQuestExpGained(int);
	EQLIB_OBJECT void EmptyPossessionSlots();
	EQLIB_OBJECT void InitializeNewPCVariables(int);
	EQLIB_OBJECT void InitPlayerStart(unsigned int, unsigned char, int);
	EQLIB_OBJECT void PrepareForRepop(int);
	EQLIB_OBJECT void ProcessAirSupply();
	EQLIB_OBJECT void ProcessFatigue();
	EQLIB_OBJECT void RefitNewbieEQ();
	EQLIB_OBJECT void RefreshMe(int);
	EQLIB_OBJECT void SetAltAbilityIndex(int, int);
	EQLIB_OBJECT void SetArmorTint(int, unsigned long);
	EQLIB_OBJECT void SetArmorType(int, int);
	EQLIB_OBJECT void SetFatigue(int);
	EQLIB_OBJECT void UnpackMyNetPC(char*, int);
	EQLIB_OBJECT void AlertInventoryChanged();
	EQLIB_OBJECT uint32_t GetCombatAbilityTimer(int, int);
	EQLIB_OBJECT void GetItemContainedRealEstateIds(ArrayClass<ItemContainingRealEstate>& Out, bool bCurrentProfileOnly = false, bool bIncludeAltStorage = true, bool bIncludeArchived = true);
	EQLIB_OBJECT void GetNonArchivedOwnedRealEstates(ArrayClass<int>& output);
	EQLIB_OBJECT EQ_Spell* GetMeleeSpellFromSkillIndex(int skillIdx);

	//----------------------------------------------------------------------------
	// Deprecated properties

	DEPRECATE("pBankArraySize is deprecated. Use BankItems.GetSize() instead.")
	inline int get_pBankArraySize() const { return BankItems.GetSize(); }
	__declspec(property(get = get_pBankArraySize)) int pBankArraySize;

	DEPRECATE("pBankArraySpec is deprecated. Use BankItems.GetContainerType() instead.")
	inline int get_pBankArraySpec() const { return BankItems.GetContainerType(); }
	__declspec(property(get = get_pBankArraySpec)) int pBankArraySpec;

	DEPRECATE("pBankArray is deprecated. Adapt the code to use BankItems instead.")
#pragma warning(suppress: 4996)
	inline deprecated::BANKARRAY* get_pBankArray() { return reinterpret_cast<deprecated::BANKARRAY*>(BankItems.ContainedItems.pItems); }
	__declspec(property(get = get_pBankArray)) deprecated::BANKARRAY* pBankArray;

	DEPRECATE("NumBankSlots is deprecated. Use BankItems.GetSize() instead.")
	inline int get_NumBankSlots() const { return BankItems.GetSize(); }
	__declspec(property(get = get_NumBankSlots)) int NumBankSlots;

	DEPRECATE("pSharedBankArray is deprecated. Adapt the code to use BankItems instead.")
#pragma warning(suppress: 4996)
	inline deprecated::SHAREDBANKARRAY* get_pSharedBankArray() { return reinterpret_cast<deprecated::SHAREDBANKARRAY*>(SharedBankItems.ContainedItems.pItems); }
	__declspec(property(get = get_pSharedBankArray)) deprecated::SHAREDBANKARRAY* pSharedBankArray;

	DEPRECATE("NumSharedSlots is deprecated. Use SharedBankItems.GetSize() instead.")
	inline int get_NumSharedSlots() const { return SharedBankItems.GetSize(); }
	__declspec(property(get = get_NumSharedSlots)) int NumSharedSlots;

	// This is used a lot, so its provided here for convenience. If you need more than this,
	// use the Group object directly.
	CGroupMember* GetGroupMember(int groupMember)
	{
		if (!Group)
			return nullptr;

		return Group->GetGroupMember(groupMember);
	}
};

// The in-memory layout of this class hierarchy looks like this:
// PcBase
// CharacterZoneClient
// PcZoneClient
// PcClient
// CharacterBase
SIZE_CHECK(PcClient, PcClient_size);

} // namespace eqlib
