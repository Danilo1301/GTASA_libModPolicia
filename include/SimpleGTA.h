#pragma once

#include "menu/SimpleGTA.h"

#include "stdint.h"

#include "rw/rwplcore.h"
#include "rw/rpworld.h"

struct IntVector2D // not original
{
    int32_t x, y;
};

class CMatrix
{
public:
    union
    {
        float f[4][4];
        struct
        {
            float rx, ry, rz, rw;
            float fx, fy, fz, fw;
            float ux, uy, uz, uw;
            float px, py, pz, pw;
        };
        struct
        {
            CVector      right;
            unsigned int flags;
            CVector      up;
            unsigned int pad1;
            CVector      at;
            unsigned int pad2;
            CVector      pos;
            unsigned int pad3;
        };
        struct // RwV3d style
        {
            CVector      m_right;
            unsigned int m_flags;
            CVector      m_forward;
            unsigned int m_pad1;
            CVector      m_up;
            unsigned int m_pad2;
            CVector      m_pos;
            unsigned int m_pad3;
        };
    };

    void* m_pAttachedMatrix;
    bool         m_bOwnsAttachedMatrix;
    char         matrixpad[3];
};

// Simple entities
class CSimpleTransform
{
public:
    CVector pos;
    float   heading;
};
class CPlaceable
{
protected:
    CPlaceable();
public:
    char padding[4];
    CSimpleTransform m_placement;
    CMatrix *m_matrix;

    CSimpleTransform* GetPlacement()
    {
        return (CSimpleTransform*)((uintptr_t)this + 4);
    }
    CMatrix* GetMatrix()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }


    CVector* GetPosSA()
    {
        auto mat = *(CMatrix**)((uintptr_t)this + 20);
        if (mat)
        {
            return &mat->pos;
        }
        return &((CSimpleTransform*)((uintptr_t)this + 4))->pos;
    }
    CVector* GetPosVC()
    {
        return (CVector*)((uintptr_t)this + 52);
    }
    CMatrix* GetMatSA()
    {
        return *(CMatrix**)((uintptr_t)this + 20);
    }
    CMatrix* GetMatVC()
    {
        return (CMatrix*)((uintptr_t)this + 4);
    }
    CMatrix* GetCamMatVC()
    {
        return (CMatrix*)this;
    }
};

class CEntity : public CPlaceable {
protected:
    CEntity();  
public:
    union {
        struct RwObject *m_pRwObject;
        struct RpClump *m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };
};

class CCamera : public CPlaceable {};
class CPed : public CEntity {};
class CVehicle : public CEntity {};
class CObject : public CPlaceable {};

struct GTAVector3D
{
    float x, y, z;
    float SqrMagnitude() { return x * x + y * y + z * z; }
    inline GTAVector3D operator-(const GTAVector3D& v) { return { x - v.x, y - v.y, z - v.z }; }
};
struct GTAMatrix
{
    GTAVector3D  right;
    unsigned int flags;
    GTAVector3D  up;
    unsigned int pad1;
    GTAVector3D  at;
    unsigned int pad2;
    GTAVector3D  pos;
    unsigned int pad3;

    void* ptr;
    bool bln;
};
struct tByteFlag
{
    uint8_t nId : 7;
    bool    bEmpty : 1;
};
struct GTAEntity
{
    inline int AsInt() { return (int)this; }
    inline int& IntAt(int off) { return *(int*)(AsInt() + off); }
    inline uint32_t& UIntAt(int off) { return *(uint32_t*)(AsInt() + off); }
    inline uint8_t& UInt8At(int off) { return *(uint8_t*)(AsInt() + off); }
    inline GTAVector3D& GetPos()
    {
        GTAMatrix* matrix = *(GTAMatrix**)(AsInt() + 20);
        return matrix ? matrix->pos : *(GTAVector3D*)(AsInt() + 4);
    }
};
struct GTAVehicleSA : GTAEntity
{
    char structure[2604];
};
struct GTAPedSA : GTAEntity
{
    char structure[1996];
    bool Player() { return UIntAt(1436) < 2; }
};