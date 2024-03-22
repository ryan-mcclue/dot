// SPDX-License-Identifier: zlib-acknowledgement

/* pros: backwards-compatible, tagless so uncompressed small (deserialisation no tag lookups so fast), 
 * flexible/conversions between formats (e.g. bool to bitfield, short to u32 etc; Operations like these tend to be difficult to perform when using tagged serialization systems like protobuf. )
 * cons: forwards-compatible, not human readable
 */

// For C++, would overload function
#define BASE_TYPE_TO_ENUM(t) (_Generic(*(t*) (NULL), \
  u8: FIELD_TYPE_U8,
  u8*: FIELD_TYPE_U8,
  u16: FIELD_TYPE_U16,
  u16*: FIELD_TYPE_U16,
  u32: FIELD_TYPE_U32,
  u32*: FIELD_TYPE_U32,
  u64: FIELD_TYPE_U64,
  u64*: FIELD_TYPE_U64,
  s8: FIELD_TYPE_S8,
  s8*: FIELD_TYPE_S8,
  s16: FIELD_TYPE_S16,
  s16*: FIELD_TYPE_S16,
  s32: FIELD_TYPE_S32,
  s32*: FIELD_TYPE_S32,
  s64: FIELD_TYPE_S64,
  s64*: FIELD_TYPE_S64,
  f32: FIELD_TYPE_F32,
  f32*: FIELD_TYPE_F32,
  f64: FIELD_TYPE_F64,
  f64*: FIELD_TYPE_F64,
  char: FIELD_TYPE_CHAR,
  bool: FIELD_TYPE_BOOL,
  bool*: FIELD_TYPE_BOOL,
  char*: FIELD_TYPE_STR,
  const char*: FIELD_TYPE_STR,
  vec2s: FIELD_TYPE_VEC2,
  vec2s*: FIELD_TYPE_VEC2,
  ivec2s FIELD_TYPE_IVEC2,
  ivec2s*: FIELD_TYPE_IVEC2,
  entity_ref_t: FIELD_TYPE_ENTITY_REF,
  entity_ref_t*: FIELD_TYPE_ENTITY_REF,
  kvstore_t: FIELD_TYPE_KVSTORE,
  kvstore_t*: FIELD_TYPE_KVSTORE,
  any_t: FIELD_TYPE_ANY,
  any_t*: FIELD_TYPE_ANY
  ))

// IMPORTANT: You don't use both the ADD and REM macro for the same field. It's one or the other. When you add a field you use ADD, when you remove the field you change it to REM. So the field is not in the struct anymore.

// https://gist.github.com/OswaldHurlem/2a19e63760cba014b9884ff58205ea95/revisions

typedef struct Serialiser Serialiser;
struct Serialiser
{
  u32 version;
  b32 is_writing;
  FILE* FilePtr; // input or input location
  String8 buffer;
  u8* buffer;
  memory_index buffer_size;  
};

INTERNAL void 
serialise(lbp_serializer* LbpSerializer, T* Datum)
{
  // keep read and write together to avoid going out of sync

  // if primitive 
  if (datum_primitive && writing)
  {
    write(sizeof(datum)
  }
  else
  {
    read(sizeof(datum))
  }
}

// if datum going to change (e.g. struct), have separate serialisation function
struct State
{
    int32_t P1Score;
    int32_t P2Score;
    int32_t P1Fouls; // Added with SV_FOULS
    int32_t P2Fouls; // Added with SV_FOULS
};

// increase anytime add or remove?
enum
{
  SV_AddedPartridge = 1,
  SV_AddedTurtleDoves,
  SV_AddedFrenchHens,
  SV_AddedCallingBirds,
  SV_AddedGoldenRings,
  SV_RemovedGoldenRings,
  // Don't remove this
  SV_LatestPlusOne
}
#define LATEST_VERSION (SV_LatestPlusOne - 1)
Serialiser global_serialiser = ZERO_STRUCT;

void
main()
{
  String8 file = read_entire_file();
  Serialiser s;
  serialise(&s);


}

void Serialize(lbp_serializer* LbpSerializer, game_score_state* Datum)
{
    Serialize(LbpSerializer, &Datum->P1Score);
    Serialize(LbpSerializer, &Datum->P2Score);

    if (LbpSerializer->DataVersion >= SV_FOULS)
    {
        Serialize(LbpSerializer, &Datum->P1Fouls);
        Serialize(LbpSerializer, &Datum->P2Fouls);
    }
}

// every field has attached version; if not present read default

#define ADD(_fieldAdded, _fieldName) \
    if (LbpSerializer->DataVersion >= (_fieldAdded)) \
    { \
        Serialize(LbpSerializer, &(Datum->_fieldName)); \
    }

void Serialize(lbp_serializer* LbpSerializer, game_score_state* Datum)
{
    ADD(SV_INITIAL, P1Score);
    ADD(SV_INITIAL, P2Score);
    ADD(SV_FOULS, P1Fouls);
    ADD(SV_FOULS, P2Fouls);
}



#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

struct lbp_serializer
{
    int32_t DataVersion;
    FILE* FilePtr;
    bool IsWriting;
};

#define VERSION_IN_RANGE(_from, _to) \
    (LbpSerializer->DataVersion >= (_from) && LbpSerializer->DataVersion < (_to))

#define ADD(_fieldAdded, _fieldName) \
    if (LbpSerializer->DataVersion >= (_fieldAdded)) \
    { \
        (LbpSerializer, &(Datum->_fieldName)); \
    }

#define ADD_LOCAL(_localAdded, _type, _localName, _defaultValue) \
    _type _localName = (_defaultValue); \
    if (LbpSerializer->DataVersion >= (_localAdded)) \
    { \
        (LbpSerializer, &(_localName)); \
    }

#define REM(_fieldAdded, _fieldRemoved, _type, _fieldName, _defaultValue) \
    _type _fieldName = (_defaultValue); \
    if (VERSION_IN_RANGE((_fieldAdded),(_fieldRemoved))) \
    { \
        (LbpSerializer, &(_fieldName)); \
    }

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void Serialize(lbp_serializer* LbpSerializer, int32_t* Datum)
{
    if (LbpSerializer->IsWriting)
    {
        fwrite(Datum, sizeof(int32_t), 1, LbpSerializer->FilePtr);
    }
    else
    {
        fread(Datum, sizeof(int32_t), 1, LbpSerializer->FilePtr);
    }
}

enum serialization_versions : int32_t
{
    SV_Scores = 1,
    SV_Fouls,
    SV_ExtraPlayers,
    SV_AllPlayersInList,
    SV_FoulsUntracked,
    // Keep this as the last element
    SV_LatestPlus1,
};

struct pbem_pong_player
{
    int32_t Points;
};

struct pbem_pong_player_list
{
    int32_t Count;
    pbem_pong_player* Ptr;
};

// (Dumb sample code) + (HMN STL Avoidance Cred) = (don't do this)
void Resize(pbem_pong_player_list* List, int32_t NewCount)
{
    if (List->Count != NewCount)
    {
        if (!List->Ptr)
        {
            List->Ptr = (pbem_pong_player*)malloc(NewCount*sizeof(pbem_pong_player));
        }
        else
        {
            List->Ptr = (pbem_pong_player*)realloc(List->Ptr, NewCount*sizeof(pbem_pong_player));
        }

        for (int32_t i = List->Count; i < NewCount; i++)
        {
            List->Ptr[i] = {};
        }

        List->Count = NewCount;
    }
}

// (Dumb sample code) + (HMN STL Avoidance Cred) = (don't do this)
void ResizeFromFront(pbem_pong_player_list* List, int32_t NewCount)
{
    if (List->Count != NewCount)
    {
        int32_t Pad = NewCount - List->Count;

        if (!List->Ptr)
        {
            List->Ptr = (pbem_pong_player*)malloc(NewCount*sizeof(pbem_pong_player));
        }
        else
        {
            List->Ptr = (pbem_pong_player*)realloc(List->Ptr, NewCount*sizeof(pbem_pong_player));

            if (Pad > 0)
            {
                memmove(List->Ptr + Pad, List->Ptr, List->Count*sizeof(pbem_pong_player));
            }
        }

        for (int32_t i = 0; i < Pad; i++)
        {
            List->Ptr[i] = {};
        }

        List->Count = NewCount;
    }
}

void Free(pbem_pong_player_list* List)
{
    free(List->Ptr);
    List->Ptr = 0;
    List->Count = 0;
}

struct pbem_pong_state
{
    pbem_pong_player_list AllPlayers;
};

void Serialize(lbp_serializer* LbpSerializer, pbem_pong_player* Datum)
{
    ADD(SV_ExtraPlayers, Points);
    REM(SV_ExtraPlayers, SV_FoulsUntracked, int32_t, Fouls, 0);
    Datum->Points = MAX(Datum->Points - Fouls, 0);
}

void Serialize(lbp_serializer* LbpSerializer, pbem_pong_player_list* Datum)
{
    ADD_LOCAL(SV_ExtraPlayers, int32_t, NewCount, Datum->Count);
    Resize(Datum, NewCount);

    for (int32_t i = 0; i < Datum->Count; i++)
    {
        ADD(SV_ExtraPlayers, Ptr[i]);
    }
}

const pbem_pong_player_list EmptyList = {};

void Serialize(lbp_serializer* LbpSerializer, pbem_pong_state* Datum)
{
    REM(SV_Scores, SV_AllPlayersInList, int32_t, P1Score, 0);
    REM(SV_Scores, SV_AllPlayersInList, int32_t, P2Score, 0);
    REM(SV_Fouls, SV_AllPlayersInList, int32_t, P1Fouls, 0);
    REM(SV_Fouls, SV_AllPlayersInList, int32_t, P2Fouls, 0);
    REM(SV_ExtraPlayers, SV_AllPlayersInList, pbem_pong_player_list, ExtraPlayers, EmptyList);

    ADD(SV_AllPlayersInList, AllPlayers);

    if (VERSION_IN_RANGE(SV_Scores, SV_AllPlayersInList))
    {
        int32_t PlayersFromLegacyFormat = ExtraPlayers.Count + 2;
        ResizeFromFront(&Datum->AllPlayers, Datum->AllPlayers.Count + PlayersFromLegacyFormat);
        Datum->AllPlayers.Ptr[0].Points = MAX(P1Score - P1Fouls, 0);
        Datum->AllPlayers.Ptr[1].Points = MAX(P2Score - P2Fouls, 0);

        for (int i = 0; i < ExtraPlayers.Count; i++)
        {
            Datum->AllPlayers.Ptr[2 + i] = ExtraPlayers.Ptr[i];
        }

        Free(&ExtraPlayers);
    }
}

void InitPongState(pbem_pong_state* PongState)
{
    Resize(&PongState->AllPlayers, 2);
}

void Gameplay(pbem_pong_state* PongState)
{
    Resize(&PongState->AllPlayers, PongState->AllPlayers.Count + 1);
    printf("A dark figure emerges from the shadows and removes its cloak. It's Player %d.\n",
        PongState->AllPlayers.Count);

    for (int Ind = 0; Ind < PongState->AllPlayers.Count; Ind++)
    {
        pbem_pong_player* Player = PongState->AllPlayers.Ptr + Ind;

        printf("Player %d has a score of %d\n", Ind+1, Player->Points);

        if (rand() % 2)
        {
            printf("Player %d fouls! But maybe this will pay off?\n", Ind+1);
            Player->Points = MAX(Player->Points - 1,  0);
        }
        else
        {
            printf("Player %d scores! But he'll need more than that to win.\n", Ind+1);
            Player->Points++;
        }

        printf("Now Player %d has a score of %d\n", Ind+1, Player->Points);
    }
}

bool SerializeIncludingVersion(lbp_serializer* LbpSerializer, pbem_pong_state* PongState)
{
    if (LbpSerializer->IsWriting)
    {
        LbpSerializer->DataVersion = SV_LatestPlus1 - 1;
    }

    (LbpSerializer, &LbpSerializer->DataVersion);

    if (LbpSerializer->DataVersion > (SV_LatestPlus1 - 1))
    {
        return false;
    }
    else
    {
        (LbpSerializer, PongState);
        return true;
    }
}

void main(int32_t, char**)
{
    int32_t latestVersion = SV_LatestPlus1 - 1;
    FILE* FilePtr;
    srand(time(NULL));

    bool Success = false;
    pbem_pong_state PongState = {};

    while (!Success)
    {
        printf("Welcome to Pong By Email v%d.\nEnter the PBEM file you'd like to open, "
            "or & to start a new game\n", latestVersion);
        char buffer[256];
        gets_s(buffer);

        if (buffer[0] == '&')
        {
            InitPongState(&PongState);
            Success = true;
        }
        else
        {
            if (FilePtr = fopen(buffer, "r"))
            {
                lbp_serializer LbpSerializer;
                LbpSerializer.IsWriting = false;
                LbpSerializer.FilePtr = FilePtr;
                if (SerializeIncludingVersion(&LbpSerializer, &PongState))
                {
                    printf("Opened PBEM file %s (from version %d)\n", buffer, LbpSerializer.DataVersion);
                    Success = true;
                }
                else
                {
                    printf("Can't read PBEM file %s from version %d\n", buffer, LbpSerializer.DataVersion);
                }
                fclose(FilePtr);
            }
            else
            {
                printf("Couldn't open %s\n", buffer);
            }
        }
    }

    Gameplay(&PongState);

    Success = false;

    while (!Success)
    {
        printf("Enter name of PBEM file to save\n");
        char buffer[256];
        gets_s(buffer);

        if (FilePtr = fopen(buffer, "w"))
        {
            lbp_serializer LbpSerializer;
            LbpSerializer.IsWriting = true;
            LbpSerializer.FilePtr = FilePtr;
            SerializeIncludingVersion(&LbpSerializer, &PongState);
            fclose(FilePtr);
            Success = true;
        }
        else
        {
            printf("Couldn't open %s\n", buffer);
        }
    }

    printf("Thanks for playing. Look forward to version %d, coming out soon!\n", SV_LatestPlus1);
}
