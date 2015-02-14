#ifndef SKETCH_3D_TEXT_H
#define SKETCH_3D_TEXT_H

#include "math/Vector3.h"

#include "system/Platform.h"

#include <map>
#include <string>
#include <vector>
using namespace std;

// Forward declaration
struct FT_LibraryRec_;
struct FT_FaceRec_;

namespace Sketch3D {

// Forward class declaration
class BufferObject;
class Shader;
class Texture2D;

/**
 * @class Text
 * Singleton class that is used to handle everything related to text rendering
 */
class SKETCH_3D_API Text {
    typedef pair<vector<int>, Texture2D*> TextureAtlas_t;

    public:
        /**
         * Destructor
         */
                                   ~Text();

        /**
         * Get the singleton's instance
         */
        static Text*                GetInstance();

        /**
         * Set the font to use for text rendering
         * @param fontName The name of the font
         * @return true if the font was correctly loaded
         */
        bool                        SetTextFont(const string& fontName);

        /**
         * Set the size of the character in pixels
         * @param width The width of the character in pixels
         * @param height The height of the character in pixels
         */
        void                        SetTextSize(size_t width, size_t height);

        /**
         * Set the text color
         * @param color The new color of the text that it will take for the next drawing
         */
        void                        SetTextColor(const Vector3& color);

        /**
         * Write the specified text on the screen, using the current font and text size
         * @param text The text to write on the screen
         * @param x The x position of the start of the text on the screen, in pixels
         * @param y The y position of the start of the text on the screen, in pixels
         */
        void                        Write(const string& text, size_t x, size_t y) const;

    private:
        static Text                 instance_;              /**< Singleton's instance */
        FT_LibraryRec_*             library_;               /**< Freetype handle */
        map<string, TextureAtlas_t> textureAtlas_;          /**< List of loaded texture atlas */
        TextureAtlas_t              currentTextureAtlas_;   /**< Current font used for font drawing */
        vector<FT_FaceRec_*>        fonts_;                 /**< List of FreeType fonts */
        FT_FaceRec_*                currentFont_;           /**< Current FreeType font */
        BufferObject*               bufferObject_;          /**< The buffer object used to draw the fonts */
        Shader*                     textShader_;            /**< The shader used to draw the text on the screen */
        size_t                      textWidth_;             /**< Text width */
        size_t                      textHeight_;            /**< Text height */
        Vector3                     textColor_;             /**< Color of the text */

        /**
         * Constructor
         */
                                    Text();

        // Disallow copy and assignement
                                    Text(const Text& src);
        Text&                       operator=(const Text& rhs);
};

}

#endif