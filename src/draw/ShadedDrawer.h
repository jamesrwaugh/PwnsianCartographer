#ifndef SHADED_DRAWER_H
#define SHADED_DRAWER_H
#include "draw/NormalDrawer.h"

/* ShadedDrawer is an extension of NormalDrawer that 
 * makes lower blocks darker to emulate a depth effect */

namespace draw
{

class ShadedDrawer : public NormalDrawer
{
protected:
    SDL_Color renderBlock(ChunkInterface& iface, int x, int z) override;
};

}

#endif
