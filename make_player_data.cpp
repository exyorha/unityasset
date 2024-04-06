#include "UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h"
#include "UnityAsset/Streams/Stream.h"
#include "UnityAsset/Streams/FileInputOutput.h"

void pack(UnityAsset::AssetBundleFile &bundle, const std::string_view &name) {
    auto content = UnityAsset::Stream(UnityAsset::readFile(std::filesystem::path("/home/reki/rein/graft/NieR_Data.bak") / name));
    bundle.entries.emplace_back(std::string(name), std::move(content), 0);
}

int main(int argc, char **argv) {

    UnityAsset::AssetBundleFile bundle;
    bundle.unityVersion = "5.x.x";
    bundle.unityRevision = "2019.4.9f1";
    bundle.blockSize = 128 * 1024;
    bundle.dataCompression = UnityAsset::UnityCompressionType::LZ4HC;
    bundle.directoryCompression = UnityAsset::UnityCompressionType::LZ4HC;

#if 0
    pack(bundle, "globalgamemanagers");
    pack(bundle, "globalgamemanagers.assets");
    pack(bundle, "sharedassets0.assets");
    pack(bundle, "sharedassets1.assets");
    pack(bundle, "sharedassets2.assets");
    pack(bundle, "level0");
    pack(bundle, "level1");
    pack(bundle, "level2");
    pack(bundle, "5bdb63a0d6b384bbface202f57359760");
    pack(bundle, "02ca98e0e81354af78bd4ddcff6b79e0");
    pack(bundle, "5896f134ccc604b6ea2cdd3b36549e0a");
    pack(bundle, "a1da11054973c40c8aec917a3e322923");
    pack(bundle, "deace60a64f3d4398a2db0f3d1a41195");
#endif
    pack(bundle, "Resources/unity default resources");
    pack(bundle, "Resources/unity_builtin_extra");

    for(const auto &entry: std::filesystem::directory_iterator("/home/reki/rein/graft/NieR_Data.bak")) {
        if(!entry.is_regular_file())
            continue;

        const auto &path = entry.path();
        if(path.extension() == ".info" || path.extension() == ".config") {
            continue;
        }

        if(path.filename().string().find(".split") != std::string::npos) {
            continue;
        }

        pack(bundle, path.filename().string());
    }

    printf("Compressing the core asset bundle, this will take some time\n");

    UnityAsset::Stream stream;
    bundle.serialize(stream);


    UnityAsset::writeFile("/home/reki/rein/graft/NieR_Data/data.unity3d", stream);
}
