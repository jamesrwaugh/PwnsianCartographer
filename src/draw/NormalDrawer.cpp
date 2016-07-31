#include "draw/NormalDrawer.h"
#include "config.h"

namespace draw
{

NormalDrawer::NormalDrawer()
{
    //Load the BlockColors to retrive color info from
    colors.load(config::get().GetString("items-zip-name"),
                config::get().GetString("items-cache-name"));
}

SDL_Color NormalDrawer::renderBlock(ChunkInterface iface, int x, int z)
{
    blocks::BlockID id = iface.getHighestSolidBlockID(x, z);
    return colors.getBlockColor(id);
}


}
