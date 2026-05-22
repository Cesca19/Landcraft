//
// Created by fran on 21/05/2026.
//

#include "BrushUtils.hpp"

namespace BrushUtils {
    void sanitizeBrushImage(sf::Image &img)
    {
        const sf::Vector2u size = img.getSize();
        if (size.x == 0 || size.y == 0) return;

        bool hasMeaningfulAlpha = false;
        for (unsigned int y = 0; y < size.y && !hasMeaningfulAlpha; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                const sf::Uint8 a = img.getPixel(x, y).a;
                if (a != 255) { // any non-opaque pixel => alpha is being used
                    hasMeaningfulAlpha = true;
                    break;
                }
            }
        }
        if (hasMeaningfulAlpha) {
            // Force RGB to white for all pixels so the shader can tint it correctly without dark artifacts.
            for (unsigned int y = 0; y < size.y; ++y) {
                for (unsigned int x = 0; x < size.x; ++x) {
                    sf::Color c = img.getPixel(x, y);
                    c.r = 255;
                    c.g = 255;
                    c.b = 255;
                    img.setPixel(x, y, c);
                }
            }
            return;
        }

        auto lumAt = [&](unsigned int x, unsigned int y) -> float {
            const sf::Color c = img.getPixel(x, y);
            return (0.299f * c.r + 0.587f * c.g + 0.114f * c.b) / 255.f;
        };
        const unsigned int maxX = size.x - 1;
        const unsigned int maxY = size.y - 1;
        const float bgLum = (lumAt(0, 0) + lumAt(maxX, 0) + lumAt(0, maxY) + lumAt(maxX, maxY)) * 0.25f;
        // If corners are dark: background is dark. If corners are gray: background is gray.
        const float bgCut = std::clamp(bgLum, 0.02f, 0.90f);
        const float denom = std::max(1e-5f, 1.0f - bgCut);

        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                constexpr float blackCut = 0.02f;
                const float lum = lumAt(x, y);
                float keyed = (lum - std::max(blackCut, bgCut)) / denom;
                keyed = std::clamp(keyed, 0.0f, 1.0f);
                // Steepen transition: keep background really transparent.
                keyed = keyed * keyed;
                const auto outA = static_cast<sf::Uint8>(std::round(keyed * 255.f));

                // Force RGB to white so only the vertex color drives the tint,
                // and to avoid dark halos when filtering.
                img.setPixel(x, y, sf::Color(255, 255, 255, outA));
            }
        }
    }
}