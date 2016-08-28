#include "draw/NormalDrawer.h"
#include "config.h"

namespace draw
{

SDL_Color NormalDrawer::renderBlock(ChunkInterface iface, int x, int z)
{
    blocks::BlockID id = iface.getHighestSolidBlockID(x, z);
    return colors.getBlockColor(id);
}

void NormalDrawer::recieveArguments(const arguments::Args& options) 
{
    BaseDrawer::recieveArguments(options);

    //Load the BlockColors to retrive color info from
    colors.load(options.itemZipFilename, "items_color_cache.json");
}


}
