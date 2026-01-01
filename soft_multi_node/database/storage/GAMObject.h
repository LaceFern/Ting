#ifndef __DATABASE_STORAGE_GAM_OBJECT_H__
#define __DATABASE_STORAGE_GAM_OBJECT_H__
#include "gallocator.h"
namespace Database{
class GAMObject {
public:
    virtual void Serialize(const GAddr& addr, GAlloc *gallocator) = 0;
    virtual void Deserialize(const GAddr& addr, GAlloc *gallocator) = 0;
};
}
#endif
