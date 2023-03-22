#include "PxeFontFile.h"

#include <fstream>

#include "PxeLogger.h"
#include "PxeEngine.h"

namespace pxengine {
	void PxeFontFile::pushFont()
	{
		ImGui::PushFont(_font);
	}

	void PxeFontFile::popFont()
	{
		ImGui::PopFont();
	}

	PXE_NODISCARD const std::filesystem::path& PxeFontFile::getPath() const
	{
		return _path;
	}

	PXE_NODISCARD PxeFontStatus PxeFontFile::getStatus() const
	{
		return _status;
	}

	PXE_NODISCARD uint16_t PxeFontFile::getLoadedSize() const
	{
		return _imFontSize;
	}

	PXE_NODISCARD uint16_t PxeFontFile::getRequestedSize() const
	{
		return _requestedSize;
	}

	PXE_NODISCARD bool PxeFontFile::getAtlasUpdateRequested() const
	{
		return (_imFontSize != _requestedSize || !_font) && _fontFileData;
	}

	void PxeFontFile::setRequestedSize(uint16_t size)
	{
		_requestedSize = size;
	}

	void PxeFontFile::addFontToAtlas(ImFontAtlas& atlas)
	{
		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		fontConfig.FontData = _fontFileData;
		if (_fontFileSize > INT_MAX) {
			PXE_FATAL("Failed to load font file, file size too big");
		}

		fontConfig.FontDataSize = _fontFileSize;
		fontConfig.SizePixels = _imFontSize = _requestedSize;
		_font = atlas.AddFont(&fontConfig);
	}

	void PxeFontFile::onDelete()
	{
		PxeEngine::getInstance().getRenderPipeline().removeFontFile(*this);
	}

	PxeFontFile::PxeFontFile(const std::filesystem::path& path) : _path(path)
	{
		_font = nullptr;
		_fontFileData = nullptr;
		_fontFileSize = 0;
		_status = PxeFontStatus::PENDING;
		_imFontSize = _requestedSize = 13; // Size of default gui font
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		_fontFileSize = file.tellg();
		if (!_fontFileSize) {
			_status = PxeFontStatus::ERROR;
			PXE_WARN("Failed load PxeFontFile, failed to get file size");
			return;
		}

		_fontFileData = malloc(_fontFileSize);
		if (!_fontFileData) {
			_status = PxeFontStatus::ERROR;
			PXE_ERROR("Failed load PxeFontFile, failed to allocate file buffer");
			return;
		}

		file.seekg(0, std::ios::beg);
		file.read((char*)_fontFileData, _fontFileSize);
		_status = PxeFontStatus::LOADED;
	}

	PxeFontFile::~PxeFontFile()
	{
		if (_fontFileData) free(_fontFileData);
	}
}