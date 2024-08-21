#include <cstdio>

#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr,
                "Usage: %s <REFERENCE ASSET FILE>\n"
                "\n"
                "This tool extracts the Unity type blobs of all used classes from the specified file and into the current directory.\n",
                argv[0]);
        return 1;
    }

    UnityAsset::SerializedAssetFile asset(UnityAsset::Stream(UnityAsset::readFile(argv[1])));

    printf("Unity version: %s, asset format version: %u, platform: %d, user data: '%s'\n", asset.unityVersion.c_str(), asset.assetVersion, asset.platform, asset.userInformation.c_str());

    for(const auto &type: asset.m_Types) {
        if(!type.m_Type.has_value()) {
            throw std::runtime_error("the file has been built without the type tree");
        }

        std::stringstream fileName;
        fileName << type.classID;

        if(type.m_ScriptID.has_value()) {
            fileName << "_";
            for(auto byte: *type.m_ScriptID) {
                fileName << std::hex;
                fileName.width(2);
                fileName.fill('0');
                fileName << static_cast<unsigned int>(byte);
            }
        }

        fileName << ".type";

        printf("Writing %s\n", fileName.str().c_str());

        UnityAsset::Stream stream;
        type.serialize(stream);
        UnityAsset::writeFile(fileName.str(), stream);
    }

    return 0;
}
