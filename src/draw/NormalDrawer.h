#ifndef NORMALDRAWER_H
#define NORMALDRAWER_H

#include "BaseDrawer.h"

namespace draw
{

class NormalDrawer : public BaseDrawer
{
protected:
    SDL_Color renderBlock(ChunkInterface iface, int x, int z) override;
    void recieveArguments(const arguments::Args& options) override;

private:
    //Item to get colors based on block IDs
    blocks::BlockColors colors;
};

}

#endif // NORMALDRAWER_H
