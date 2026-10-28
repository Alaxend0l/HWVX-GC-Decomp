#include "common.h"

extern f32 g_fDeltaTimeSec;
extern s32 g_iDeltaTimeMS;
extern Vector4 g_zeroVector;

u8 FloatsEqual(f32 a, f32 b, f32 eps);
f32 _fabs(f32 x);
double acos(double x);
f32 Sqrt(f32 x);
f32 MagnitudeVector(Vector4 *v);
f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
f32 PSVECSquareMag(Vector4 *v);
void PSVECNormalize(Vector4 *src, Vector4 *dst);
f32 PSVECSquareDistance(Vector4 *a, Vector4 *b);
void GenerateQuaternion(Vector4 *axis, Quaternion *q, f32 angle);
void QuaternionMult(Quaternion *a, Quaternion *b, Quaternion *out);
void QuaternionToSubMatrix(Quaternion *quat, u8 *mtx);
void InverseXformVec(u8 *mtx, Vector4 *in, Vector4 *out);
s32 ThingManager_GetActiveCars(u8 **cars, u32 max);
u8 *ThingManager_DARYL_CreateSlave(s32 type, void *params, u32 id);
void ThingManagerList_Append(void *list, u8 *thing);
void Projectile_Release(u8 *proj);

typedef struct SmartProjParams {
    u8 unk0;
    u8 invertZ;
    u8 pad2[0x4 - 0x2];
    f32 turnMax;
    f32 turnOut;
    f32 turnIn;
    f32 dropRange;
    f32 gain;
    u8 pad18[0x1C - 0x18];
    f32 fov;
    f32 minRange;
} SmartProjParams;

typedef struct SmartProjInfo {
    u8 pad0[0x1C];
    s32 damage;
    u8 pad20[0x34 - 0x20];
    f32 f34;
    u8 pad38[0x40 - 0x38];
    f32 speedGain;
    f32 maxSpeed;
    u8 pad48[0x74 - 0x48];
    u32 slaveId;
    u8 pad78[0xA8 - 0x78];
    SmartProjParams *params;
} SmartProjInfo;

typedef struct SmartTarget {
    Vector4 vec00;
    u8 mtx[0x20];
    u8 pad30[0x40 - 0x30];
    Vector4 pos;
    u8 pad50[0x88 - 0x50];
    u32 list;
    void **vtable;
} SmartTarget;

typedef struct SmartProjVTable {
    u8 pad0[0x18];
    void (*applyImpulse)(SmartTarget *obj, void *hit, f32 impulse, s32 damage);
    u8 pad1C[0x3C - 0x1C];
    void (*getPos)(SmartTarget *obj, Vector4 *out);
} SmartProjVTable;

typedef struct CreateSlaveParams {
    Vector4 a;
    Quaternion b;
    Vector4 c;
    Vector4 zero;
    u32 id;
    s32 player;
    f32 f0;
    f32 f1;
} CreateSlaveParams;

typedef struct HitRecord {
    Vector4 pos;
    Vector4 normal;
    u8 *thing;
    u8 *other;
    s32 time;
    u8 pad2C[0x50 - 0x2C];
} HitRecord;

typedef struct SmartProjectile {
    Vector4 vel;
    u8 mtx[0x20];
    Vector4 dir;
    Vector4 pos;
    Quaternion quat;
    u8 pad60[0x74 - 0x60];
    u32 flags;
    u8 pad78[0x94 - 0x78];
    s32 player;
    u8 pad98[0x120 - 0x98];
    SmartProjInfo *info;
    u8 pad124[0x14C - 0x124];
    SmartTarget *target;
    f32 turnX;
    f32 turnY;
    f32 turnZ;
    u16 hits15C;
    s16 timer;
} SmartProjectile;

s32 SmartProjectileA_PreMove(SmartProjectile *proj){CreateSlaveParams p;HitRecord h;u8*cars[16];Vector4 cross;Vector4 axis;Quaternion quat;Quaternion result;Vector4 dir;Vector4 local;Vector4 aim;u8*s;u32 id,cnt;u32 i;s8 fnd;f32 speed,dot,ang,inv,ns,dist,lead;f32 mt,tO,tI,gain; speed=MagnitudeVector(&proj->vel);PSVECCrossProduct(&proj->vel,&proj->dir,&cross); if(!FloatsEqual(MagnitudeVector(&cross),0.0f,0.05f)){inv=1.0f/speed;dir.x=proj->vel.x*inv;dir.y=proj->vel.y*inv;dir.z=proj->vel.z*inv;dot=PSVECDotProduct(&dir,&proj->dir);if(_fabs(dot)<0.99f){ang=(f32)acos((double)dot);PSVECCrossProduct(&proj->dir,&dir,&axis);GenerateQuaternion(&axis,&quat,ang);QuaternionMult(&quat,&proj->quat,&result);proj->quat=result;QuaternionToSubMatrix(&proj->quat,&proj->mtx[0]);}} ns=proj->info->speedGain*g_fDeltaTimeSec+speed;if(ns>proj->info->maxSpeed)ns=proj->info->maxSpeed;proj->timer-=(u16)g_iDeltaTimeMS;if(proj->timer>0){proj->timer=-1;return 0;} if(proj->target){tO=proj->info->params->turnOut;tI=proj->info->params->turnIn;mt=proj->info->params->turnMax;dist=Sqrt(PSVECSquareDistance(&proj->pos,&proj->target->pos));if(ns!=0.0f)lead=dist/ns;else lead=0.0f;((SmartProjVTable*)proj->target->vtable)->getPos(proj->target,&aim);if(lead>2.0f)lead=0.3f*(lead-2.0f)+2.0f;if(lead>0.2f)lead=0.2f;aim.x+=proj->target->vec00.x*lead;aim.y+=proj->target->vec00.y*lead;aim.z+=proj->target->vec00.z*lead;InverseXformVec(&proj->mtx[0],&aim,&local); if(local.z<0.0f){cnt=ThingManager_GetActiveCars(cars,16);fnd=0;for(i=0;i<cnt;i++)if(cars[i]==(u8*)proj->target){fnd=1;break;} if(fnd&&dist<proj->info->f34){id=proj->info->slaveId;if(id){p.id=id;p.b=proj->quat;p.player=proj->player;p.f0=0.0f;p.f1=0.0f;p.zero=g_zeroVector;InverseXformVec(&proj->target->mtx[0],&proj->pos,&p.a);s=ThingManager_DARYL_CreateSlave(1,&p,0xFF);if(s){s[0x83]=0;ThingManagerList_Append(&proj->target->list,s);}} h.thing=(u8*)proj;h.other=(u8*)proj->target;h.time=0;h.pos=proj->pos;PSVECSubtract(&proj->pos,&proj->target->pos,&h.normal);if(PSVECSquareMag(&h.normal))PSVECNormalize(&h.normal,&h.normal);((SmartProjVTable*)proj->target->vtable)->applyImpulse(proj->target,&h,0.0f,proj->info->damage);Projectile_Release((u8*)proj);proj->flags|=2;}else proj->target=NULL;} else{if(dist>proj->info->params->dropRange){proj->turnZ=0.8f;local.x/=local.z;local.y/=local.z;if(proj->turnX*local.x<0.0f)proj->turnX+=g_fDeltaTimeSec*(tI*local.x);else proj->turnX+=g_fDeltaTimeSec*(tO*local.x);if(proj->turnY*local.y>0.0f)proj->turnY+=g_fDeltaTimeSec*(-tI*local.y);else proj->turnY+=g_fDeltaTimeSec*(-tO*local.y);}else if(local.z>0.0f){gain=proj->info->params->gain;inv=1.0f/local.z;proj->turnX=inv*(g_fDeltaTimeSec*(gain*local.x));proj->turnY=inv*(g_fDeltaTimeSec*(-gain*local.y));proj->turnZ=0.0f;}if(proj->turnX>mt)proj->turnX=mt;if(proj->turnX<-mt)proj->turnX=-mt;if(proj->turnY>mt)proj->turnY=mt;if(proj->turnY<-mt)proj->turnY=-mt;}} else{proj->turnZ=0.0f;proj->turnY=0.0f;proj->turnX=0.0f;} proj->vel.x=proj->dir.x*ns;proj->vel.y=proj->dir.y*ns;proj->vel.z=proj->dir.z*ns;return 1;}
