#include <cstdio>

#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/Streams/Stream.h>

#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>

int main(int argc, char **argv) {

    if(argc < 3) {
        fprintf(stderr, "Usage: %s <FILE NAME> <REWRITTEN FILE NAME>\n", argv[0]);
        return 1;
    }

    UnityAsset::AssetBundleFile bundle(UnityAsset::Stream(UnityAsset::readFile(argv[1])));

    printf(
        "Unity version:  %s\n"
        "Unity revision: %s\n"
        "Directory compression type: %u, data compression type: %u, block size (heuristic!): %zu\n"
        "Asset bundle CRC: %u\n",
        bundle.unityVersion.c_str(), bundle.unityRevision.c_str(),
        static_cast<uint32_t>(bundle.directoryCompression),
        static_cast<uint32_t>(bundle.dataCompression),
        bundle.blockSize,
        bundle.assetBundleCRC);

    for(const auto &entry: bundle.entries) {
        printf("file '%s': flags %u, %zu bytes\n",
               entry.filename().c_str(), entry.flags(), entry.data().length());
    }

    UnityAsset::Stream rewritten;
    bundle.serialize(rewritten);

    UnityAsset::writeFile(argv[2], rewritten);
}
