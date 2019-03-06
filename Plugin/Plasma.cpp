#include <cstdint>
#include <cmath>
#include "Unity/IUnityRenderingExtensions.h"

namespace
{
    // Old school plasma effect
    uint32_t Plasma(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t frame)
    {
        auto px = (float)x / width;
        auto py = (float)y / height;
        auto time = frame / 60.0f;

        auto l = sinf(px * sinf(time * 1.3f) + sinf(py * 4 + time) * sinf(time));

        auto r = (uint32_t)(sinf(l *  6) * 127 + 127);
        auto g = (uint32_t)(sinf(l *  7) * 127 + 127);
        auto b = (uint32_t)(sinf(l * 10) * 127 + 127);

        return r + (g << 8) + (b << 16) + 0xff000000;
    }

	uint32_t* g_img = NULL;
    // Callback for texture update events
    void TextureUpdateCallback(int eventID, void* data)
    {
        auto event = static_cast<UnityRenderingExtEventType>(eventID);

        if (event == kUnityRenderingExtEventUpdateTextureBeginV2)
        {
            // UpdateTextureBegin: Generate and return texture image data.
            auto params = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
            auto frame = params->userData;

            for (auto y = 0u; y < params->height; y++)
                for (auto x = 0u; x < params->width; x++)
					g_img[y * params->width + x] = Plasma(x, y, params->width, params->height, frame);

            params->texData = g_img;
        }
        else if (event == kUnityRenderingExtEventUpdateTextureEndV2)
        {
            // UpdateTextureEnd: Free up the temporary memory.
            //auto params = reinterpret_cast<UnityRenderingExtTextureUpdateParamsV2*>(data);
            //delete[] reinterpret_cast<uint32_t*>(params->texData);
        }
    }
}

extern "C" UnityRenderingEventAndData UNITY_INTERFACE_EXPORT GetTextureUpdateCallback()
{
    return TextureUpdateCallback;
}


extern "C" void UNITY_INTERFACE_EXPORT CreatePlasma(int width,int height)
{
	if (g_img == NULL)
		g_img = new uint32_t[width * height];
}

extern "C" void UNITY_INTERFACE_EXPORT DestroyPlasma()
{
	if (g_img != NULL)
		delete[] g_img;
}
