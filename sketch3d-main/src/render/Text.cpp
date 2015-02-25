#include "render/Text.h"

#include "render/BufferObject.h"
#include "render/BufferObjectManager.h"
#include "render/Renderer.h"
#include "render/Shader.h"
#include "render/Texture2D.h"

#include "system/Logger.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Sketch3D {
Text Text::instance_;

Text::Text() : library_(nullptr), currentFont_(nullptr), bufferObject_(nullptr), textShader_(nullptr), textWidth_(32), textHeight_(32), textColor_(1.0f, 1.0f, 1.0f),
               bufferText_(false)
{
    FT_Error error = FT_Init_FreeType(&library_);
    if (error != FT_Err_Ok) {
        Logger::GetInstance()->Warning("Couldn't initialize text rendering");
    }
}

Text::~Text() {
    map<string, TextureAtlas_t>::iterator it = textureAtlas_.begin();
    for (; it != textureAtlas_.end(); ++it) {
        delete it->second.second;
    }

    for (size_t i = 0; i < fonts_.size(); i++) {
        FT_Done_Face(fonts_[i]);
    }

    FT_Done_FreeType(library_);
}

Text* Text::GetInstance() {
    return &instance_;
}

bool Text::SetTextFont(const string& fontName) {
    // We initialize the buffer object here because we have to wait for the backend API to initialize
    if (bufferObject_ == nullptr) {
        VertexAttributesMap_t vertexAttributes;
        vertexAttributes[VERTEX_ATTRIBUTES_POSITION] = 0;
        vertexAttributes[VERTEX_ATTRIBUTES_TEX_COORDS] = 1;
        bufferObject_ = Renderer::GetInstance()->GetBufferObjectManager()->CreateBufferObject(vertexAttributes);
    }

    map<string, TextureAtlas_t>::iterator it = textureAtlas_.find(fontName);
    if (it != textureAtlas_.end()) {
        currentTextureAtlas_ = it->second;
        return true;
    }

    FT_Face face;
    FT_Error error = FT_New_Face(library_, fontName.c_str(), 0, &face);
    if (error != FT_Err_Ok) {
        Logger::GetInstance()->Warning("Couldn't load font : " + fontName);
        return false;
    }

    fonts_.push_back(face);
    currentFont_ = face;
    FT_GlyphSlot slot = currentFont_->glyph;
    int maxHeight = 0;
    int combinedWidth = 0;

    FT_Set_Pixel_Sizes(currentFont_, 0, 32);

    // Get the maximum height of the glyph for the texture
    for (int i = 33; i < 127; i++) {
        error = FT_Load_Char(currentFont_, (char)i, FT_LOAD_RENDER);
        if (error != FT_Err_Ok) {
            continue;
        }

        if (slot->bitmap.rows > maxHeight) {
            maxHeight = slot->bitmap.rows;
        }

        combinedWidth += slot->bitmap.width;
    }

    size_t alignedWidth = ((combinedWidth + 3) / 4) * 4;

    Texture2D* textureFont = Renderer::GetInstance()->CreateTexture2D();
    textureFont->SetWidth(alignedWidth);
    textureFont->SetHeight(maxHeight);
    textureFont->SetTextureFormat(TEXTURE_FORMAT_GRAYSCALE);
    textureFont->Create();

    unsigned char* data = new unsigned char[alignedWidth * maxHeight];
    memset(data, 0, alignedWidth * maxHeight);
    size_t lastWidth = 0;
    size_t idx = 0;

    TextureAtlas_t textureAtlas;
    textureAtlas.first.reserve(127 - 33);

    // Load all the glyph in the texture atlas
    for (int i = 33; i < 127; i++) {
        error = FT_Load_Char(currentFont_, (char)i, FT_LOAD_RENDER);
        if (error != FT_Err_Ok) {
            continue;
        }
        size_t data_idx = 0;
        size_t bitmap_idx = 0;

        for (int j = 0; j < slot->bitmap.rows; j++) {
            for (int k = 0; k < slot->bitmap.width; k++) {
                data_idx = lastWidth + (j * alignedWidth + k);
                data[data_idx] = slot->bitmap.buffer[(slot->bitmap.rows - j - 1) * slot->bitmap.width + k];
            }
        }

        textureAtlas.first.push_back(lastWidth);
        lastWidth += slot->bitmap.width;
    }

    textureFont->SetPixelDataBytes(data, alignedWidth, maxHeight);
    delete[] data;

    textureAtlas.second = textureFont;

    textureAtlas_[fontName] = textureAtlas;
    currentTextureAtlas_ = textureAtlas;

    return true;
}

void Text::SetTextSize(size_t width, size_t height) {
    textWidth_ = width;
    textHeight_ = height;
}

void Text::SetTextColor(const Vector3& color) {
    textColor_ = color;
}

void Text::Write(const string& text, size_t x, size_t y) {
    if (currentFont_ == nullptr) {
        Logger::GetInstance()->Warning("No font loaded in Text::Write");
        return;
    }

    FT_Vector pen = { x, y };
    FT_GlyphSlot slot = currentFont_->glyph;
    FT_Error error;

    float screenWidth = (float)Renderer::GetInstance()->GetScreenWidth();
    float screenHeight = (float)Renderer::GetInstance()->GetScreenHeight();
    float halfScreenWidth = screenWidth * 0.5f;
    float halfScreenHeight = screenHeight * 0.5f;

    Texture2D* textureFont = currentTextureAtlas_.second;

    float yScale = (float)textHeight_ / (float)textureFont->GetHeight();

    for (size_t i = 0; i < text.size(); i++) {
        error = FT_Load_Char(currentFont_, text[i], FT_LOAD_RENDER);
        if (error != FT_Err_Ok) {
            Logger::GetInstance()->Warning("Couldn't load character : " + to_string(text[i]) + " from font : " + currentFont_->family_name + " in Text::Write");
            return;
        }

        float xScale = (float)textWidth_ / 30.0f;

        if (text[i] == ' ') {
            // We shift by 6 to the right because the advance value is expressed in the 1/64th of a pixel
            pen.x += (FT_Pos)((float)(slot->advance.x >> 6) * xScale);
            continue;
        }

        float xPos = (float)(pen.x + slot->bitmap_left * xScale);
        float yPos = (float)Renderer::GetInstance()->GetScreenHeight() - (float)(pen.y + textureFont->GetHeight() * yScale);
        float u = ((float)currentTextureAtlas_.first[(int)text[i] - (int)'!']) / (float)textureFont->GetWidth();
        float uWidth = (float)slot->bitmap.width / (float)textureFont->GetWidth();

        textVertices_.push_back(xPos / halfScreenWidth - 1.0f); textVertices_.push_back(yPos / halfScreenHeight - 1.0f); textVertices_.push_back(0.0f);
        textVertices_.push_back(u); textVertices_.push_back(0.0f);

        textVertices_.push_back(xPos / halfScreenWidth - 1.0f); textVertices_.push_back((yPos + textureFont->GetHeight() * yScale)/ halfScreenHeight - 1.0f); textVertices_.push_back(0.0f);
        textVertices_.push_back(u); textVertices_.push_back(1.0f);

        textVertices_.push_back((xPos + slot->bitmap.width * xScale) / halfScreenWidth - 1.0f); textVertices_.push_back((yPos + textureFont->GetHeight() * yScale) / halfScreenHeight - 1.0f); textVertices_.push_back(0.0f);
        textVertices_.push_back(u + uWidth); textVertices_.push_back(1.0f);

        textVertices_.push_back((xPos + slot->bitmap.width * yScale) / halfScreenWidth - 1.0f); textVertices_.push_back(yPos / halfScreenHeight - 1.0f); textVertices_.push_back(0.0f);
        textVertices_.push_back(u + uWidth); textVertices_.push_back(0.0f);

        unsigned short idx = 0;
        if (!textIndices_.empty()) {
            idx = textIndices_.back() + ((Renderer::GetInstance()->GetCullingMethod() == CULLING_METHOD_BACK_FACE) ? 2 : 1);
        }

        if (Renderer::GetInstance()->GetCullingMethod() == CULLING_METHOD_BACK_FACE) {
            textIndices_.push_back(idx); textIndices_.push_back(idx + 2); textIndices_.push_back(idx + 1);
            textIndices_.push_back(idx); textIndices_.push_back(idx + 3); textIndices_.push_back(idx + 2);
        } else {
            textIndices_.push_back(idx); textIndices_.push_back(idx + 1); textIndices_.push_back(idx + 2);
            textIndices_.push_back(idx); textIndices_.push_back(idx + 2); textIndices_.push_back(idx + 3);
        }
        
        // We shift by 6 to the right because the advance value is expressed in the 1/64th of a pixel
        pen.x += (FT_Pos)((float)(slot->advance.x >> 6) * xScale);
    }

    if (!bufferText_) {
        bufferObject_->SetVertexData(textVertices_, VERTEX_ATTRIBUTES_POSITION | VERTEX_ATTRIBUTES_TEX_COORDS);
        bufferObject_->SetIndexData(&textIndices_[0], textIndices_.size());
        textVertices_.clear();
        textIndices_.clear();

        Renderer::GetInstance()->DrawTextBuffer(bufferObject_, textureFont, textColor_);
    }
}

void Text::BeginTextGroup() {
    if (bufferText_) {
        Logger::GetInstance()->Warning("Already called BeginTextGroup");
    }

    bufferText_ = true;
}

void Text::EndTextGroup() {
    Texture2D* textureFont = currentTextureAtlas_.second;

    bufferObject_->SetVertexData(textVertices_, VERTEX_ATTRIBUTES_POSITION | VERTEX_ATTRIBUTES_TEX_COORDS);
    bufferObject_->SetIndexData(&textIndices_[0], textIndices_.size());
    textVertices_.clear();
    textIndices_.clear();

    Renderer::GetInstance()->DrawTextBuffer(bufferObject_, textureFont, textColor_);

    bufferText_ = false;
}

}