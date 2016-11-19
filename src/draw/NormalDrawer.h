#ifndef NORMALDRAWER_H
#define NORMALDRAWER_H
#include "BaseDrawer.h"

/* NormalDrawer is the normal color drawer. Output is the block's color */

namespace draw
{

class NormalDrawer : public BaseDrawer
{
protected:
    SDL_Color renderBlock(ChunkInterface& iface, int x, int z) override;
    void recieveArguments(const arguments::Args& options) override;

private:
    //Item to get colors based on block IDs
    blocks::BlockColors colors;
};

}

#endif
