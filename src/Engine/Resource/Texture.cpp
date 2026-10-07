#include "Engine/Resource/Texture.h"

#include <utility>
#include <Windows.h>
#include <wincodec.h>
#include <wrl/client.h>

namespace MyGameEngine
{
    Texture::Texture(std::filesystem::path filePath) : filePath_(std::move(filePath))
    {
        if (!std::filesystem::exists(filePath_)) return;
        const HRESULT comResult = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
        Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
        Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                    IID_PPV_ARGS(&factory))) ||
            FAILED(factory->CreateDecoderFromFilename(filePath_.c_str(), nullptr, GENERIC_READ,
                                                      WICDecodeMetadataCacheOnDemand, &decoder)) ||
            FAILED(decoder->GetFrame(0, &frame)) || FAILED(factory->CreateFormatConverter(&converter)) ||
            FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA,
                                         WICBitmapDitherTypeNone, nullptr, 0.0,
                                         WICBitmapPaletteTypeCustom)) ||
            FAILED(converter->GetSize(&width_, &height_)))
        {
            width_ = height_ = 0;
        }
        else
        {
            pixels_.resize(static_cast<std::size_t>(width_) * height_ * 4);
            if (FAILED(converter->CopyPixels(nullptr, width_ * 4,
                                             static_cast<UINT>(pixels_.size()), pixels_.data())))
            {
                pixels_.clear(); width_ = height_ = 0;
            }
        }
        if (SUCCEEDED(comResult)) CoUninitialize();
    }

    const std::filesystem::path& Texture::GetFilePath() const noexcept
    {
        return filePath_;
    }

    bool Texture::IsLoaded() const noexcept { return !pixels_.empty(); }
    std::uint32_t Texture::GetWidth() const noexcept { return width_; }
    std::uint32_t Texture::GetHeight() const noexcept { return height_; }
    const std::vector<std::uint8_t>& Texture::GetPixels() const noexcept { return pixels_; }
} // namespace MyGameEngine
