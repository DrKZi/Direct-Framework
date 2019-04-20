#pragma once

#include "D3D11_Framework.h"

using namespace D3D11Framework;

class InfoLable
{
    struct Vertex
    {
        XMFLOAT3 pos;
        XMFLOAT2 tex;
    };
public:
    class InfoLable();
    bool Init(ID3D11Device*, Shader *shader, int screenW, int screenH, int width, int height);
    void Close();
    void Render(ID3D11DeviceContext*);
    void Clear();

    RenderTarget* GetRT();
    Image* GetPhoto();
    std::vector<Text*> GetText();
	float GetImgWidth();
	float GetImgHeight();

    bool isPhoto();
    bool equilPhoto(std::wstring fileName);

    void SetPhoto(std::wstring file, Image *imageObj, float height);
    void SetText(std::vector<Text *> text);

private:
    bool m_initShder();

    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    int m_screenWidth, m_screenHeight;
    int m_bitmapWidth, m_bitmapHeight;
    int m_previousPosX, m_previousPosY;

    Shader* m_shader;

    Image * photo;
    std::wstring m_photoFile;
    std::vector<Text *> txt;

    RenderTarget *m_rt;
};