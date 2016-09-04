#include <fstream>
#include "json11.hpp"
#include "ZipLib/ZipFile.h"
#include "utility/lodepng.h"
#include "utility/utility.h"
#include "blocks/blocks.h"

namespace
{

/* This is a std::map comparison functor that returns
 * true if two SDL_Colors are similar. By doing this,
 * similar colors can be grouped together */

struct SimilarColorCompare
{
    static const int tol = 20;

    bool operator()(const SDL_Color& a, const SDL_Color& b)
    {
        return (abs(a.r-b.r) < tol) && (abs(a.g-b.g) < tol) && (abs(a.b-b.b) < tol);
    }
};

}

namespace blocks
{

BlockID::BlockID(unsigned id, unsigned meta)
    : id(id), meta(meta)
{

}

BlockID::BlockID(const std::string& string)
{
    //Example: "405-3.png" or "405-3"
    size_t dashpos = string.find('-'),
           dotpos  = string.find('.');

    /* There's no checks for == npos here, because by default
     * if they're not found, they'll be npos anyway. In that case,
     * we have something like "404" which is the ID, and meta
     * can default into 0 */
    id = std::atoi(string.substr(0,dashpos).c_str());

    if(dashpos != std::string::npos) {
        meta = std::atoi(string.substr(dashpos+1, dotpos).c_str());
    }
}

bool BlockID::operator<(const BlockID& other) const
{
   if(id < other.id)
       return true;
   else if(id == other.id)
       return (meta < other.meta);
   return false;
}

bool BlockID::operator==(const BlockID& other) const
{
    return (id == other.id) && (meta == other.meta);
}

bool BlockID::operator!=(const BlockID& other) const
{
    return !(*this == other);
}

BlockID::operator std::string() const
{
    std::stringstream ss;
    ss << id << '-' << meta;
    return ss.str();
}

}

namespace blocks
{

BlockColors::BlockColors()
{
    rgba = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
}

BlockColors::~BlockColors()
{
    SDL_FreeFormat(rgba);
}

void BlockColors::load(const std::string& zipFileName, const std::string& cacheFileName)
{
    using namespace json11;

    //Keep filenames for later
    this->zipFileName = zipFileName;
    this->cacheFileName = cacheFileName;

    //Zip file interface
    auto archive = ZipFile::Open(zipFileName);

    //Cache JSON. This is a single { } of key:value of "blockid-meta" to .zip CRC and RGBA color
    // ex: {"2-4": {"crc": 5234231, "color": 2489974272}, ... }
    std::string parseErr;
    Json cacheJson = Json::parse(readFile(cacheFileName), parseErr);
    if(!parseErr.empty()) {
        log("Could not read cache \"", cacheFileName, "\": ", parseErr);
    }

    /* Will be true if computeColor is called, basically if cache is missing
     * or CRC changed in zip for any file */
    bool hadToRecompute = false;

    for(unsigned i = 0; i != archive->GetEntriesCount(); ++i)
    {
        /* Name and CRC of the block image in .zip
         * substr on the name is used to cut off the .png at the end */
        auto entry = archive->GetEntry(i);
        unsigned zipcrc = entry->GetCrc32();
        std::string name = entry->GetName();
        name = name.substr(0, name.find('.'));

        /* To get a block color, first the cache (cacheFileName, a .json file) is checked.
         * if it's not there, it's recomputed used computecolor. If that happens,
         * "hadToRecompute" is set to true, and a new .json cache will be written out */
        Json key = cacheJson[name];
        if(!key.is_object()) {
            //If key is not found, look for key with a 0 meta. e.g, "404-0" and not "404"
            std::string nameWithMeta = name + "-0";
            key = cacheJson[nameWithMeta];
        }

        SDL_Color blockColor;

        if(key.is_object() && (unsigned)key["crc"].int_value() == zipcrc) {
            Uint32 cachePixel = key["color"].int_value();
            SDL_GetRGBA(cachePixel, rgba, &blockColor.r, &blockColor.g, &blockColor.b, &blockColor.a);
        } else {
            blockColor = computeColor(entry);
            hadToRecompute = true;
        }

        //Store color and CRC in this object
        blockColors[name] = {blockColor, zipcrc};
    }

    //If any blocks were not found in cache
    if(hadToRecompute) {
        saveNewJsonCache();
    }
}

bool BlockColors::isLoaded() const
{
    return isLoaded();
}

SDL_Color BlockColors::computeColor(const ZipArchiveEntry::Ptr& blockImage)
{
    /* computeColor method: For each non-transparent pixel,
     * count the number of times the pixel color has appeared, grouping
     * similarly-colored pixels. The highest counted is the color for the block*/

    //Get the raw PNG data from the .zip, and decode it into pixels
    std::vector<char> pngBytes = readZipEntry(blockImage);
    std::vector<unsigned char> pixels;
    unsigned w = 0, h = 0;
    lodepng::decode(pixels, w, h, (const unsigned char*)pngBytes.data(), pngBytes.size());

    /* A map to map a {color range -> use counts}. These are a number of "buckets"
     * that colors are grouped into to find the most used. By using SimilarColorCompare
     * as the comparison, similar colors are said to be equal, effectively grouping them
     * in the map */
    std::map<SDL_Color, unsigned, SimilarColorCompare> colorCounts;

    /* Add colors over the pixels that are not transparent
     * The recorded color is the RGA value with 255 alpha, for now,
     * To prevent solid blocks like grass having transparency because the
     * first zero-alpha pixel being not 255 alpha */
    for(unsigned i = 0; i != pixels.size(); i += 4)
    {
        Uint8 r = pixels[i+0],
         g = pixels[i+1],
         b = pixels[i+2],
         a = pixels[i+3];
        if(a != SDL_ALPHA_TRANSPARENT) {
            colorCounts[SDL_Color{r,g,b,SDL_ALPHA_OPAQUE}] += 1;
        }
    }

    //Get iterator to element in map with most use counts
    auto it = std::max_element(colorCounts.begin(), colorCounts.end(),
        [](auto& pair0, auto& pair1) { return pair0.second < pair1.second; });

    //Return key type at that iterator; the color
    return it->first;
}

const SDL_PixelFormat* BlockColors::pixelFormat() const
{
    return rgba;
}

std::vector<char> BlockColors::readZipEntry(const ZipArchiveEntry::Ptr& blockImage)
{
    //This is different from readStream, size needs to be gotten this way
    size_t size = blockImage->GetSize();
    std::vector<char> content(size);

    //Read "size" bytes into string
    std::istream* stream = blockImage->GetDecompressionStream();
    stream->read(content.data(), size);

    return content;
}

void BlockColors::saveNewJsonCache() const
{
    std::ofstream file(cacheFileName);
    if(!file.is_open()) {
        error("Could not open ", cacheFileName, " for writing new cahce");
    }

    json11::Json::object root;

    for(const auto& pair : blockColors)
    {
        std::string id = pair.first; //BlockID -> string conversion
        auto&& value = pair.second;

        //Convience
        SDL_Color color = value.first;
        unsigned crc = value.second;
        Uint32 pixel = SDL_MapRGBA(rgba, color.r, color.g, color.b, color.a);

        root.insert(
            { id, json11::Json::object{{"crc",(int)crc}, {"color",(int)pixel}} }
        );
    }

    file << json11::Json(root).dump() << std::endl;

    file.close();
}

SDL_Color BlockColors::getBlockColor(unsigned id, unsigned meta) const
{
    return getBlockColor(BlockID{id,meta});
}

SDL_Color BlockColors::getBlockColor(const BlockID& blockid) const
{
    auto it = blockColors.find(blockid);

    if(it != blockColors.end())  {
        return it->second.first;
    } else {
        /* Base recursive case, if meta 0 isn't found, we safely
         * say we don't know the block */
        if(blockid.meta == 0) {
            return {255, 20, 147, 255}; //Unknown color
        }

        /* If not found, check if we have the block with no metadata.
         * This helps cases like crops and rotated stairs which metadata
         * doesn't determine color, only break color*/
        return getBlockColor(blockid.id, 0);
    }
}

}
