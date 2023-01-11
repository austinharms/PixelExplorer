#include "PxeRenderMaterial.h"

#include "NpLogger.h"

namespace pxengine {
	PxeRenderMaterial::PxeRenderMaterial(PxeShader& shader) : _shader(shader) {
		shader.grab();
		_lastShaderCount = 0;
		if (shader.getAssetStatus() == PxeGLAssetStatus::ERROR) {
			PXE_WARN("Created PxeRenderMaterial with invalid PxeShader");
		}
	}

	PxeRenderMaterial::~PxeRenderMaterial()
	{
		_shader.drop();
	}

	void PxeRenderMaterial::setProperty1f(const std::string& name, const float value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::FLOAT1, PxeRenderMaterialValue::PxePropertyValue{ .f = value });
	}

	void PxeRenderMaterial::setProperty2f(const std::string& name, const glm::vec2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::FLOAT2, PxeRenderMaterialValue::PxePropertyValue{ .fvec2 = value });
	}

	void PxeRenderMaterial::setProperty3f(const std::string& name, const glm::vec3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::FLOAT3, PxeRenderMaterialValue::PxePropertyValue{ .fvec3 = value });
	}

	void PxeRenderMaterial::setProperty4f(const std::string& name, const glm::vec4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::FLOAT4, PxeRenderMaterialValue::PxePropertyValue{ .fvec4 = value });
	}

	void PxeRenderMaterial::setProperty1i(const std::string& name, const int32_t value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::INT1, PxeRenderMaterialValue::PxePropertyValue{ .i32 = value });
	}

	void PxeRenderMaterial::setProperty2i(const std::string& name, const glm::i32vec2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::INT2, PxeRenderMaterialValue::PxePropertyValue{ .i32vec2 = value });
	}

	void PxeRenderMaterial::setProperty3i(const std::string& name, const glm::i32vec3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::INT3, PxeRenderMaterialValue::PxePropertyValue{ .i32vec3 = value });
	}

	void PxeRenderMaterial::setProperty4i(const std::string& name, const glm::i32vec4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::INT4, PxeRenderMaterialValue::PxePropertyValue{ .i32vec4 = value });
	}

	void PxeRenderMaterial::setProperty1ui(const std::string& name, const uint32_t value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::UINT1, PxeRenderMaterialValue::PxePropertyValue{ .ui32 = value });
	}

	void PxeRenderMaterial::setProperty2ui(const std::string& name, const glm::u32vec2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::UINT2, PxeRenderMaterialValue::PxePropertyValue{ .ui32vec2 = value });
	}

	void PxeRenderMaterial::setProperty3ui(const std::string& name, const glm::u32vec3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::UINT3, PxeRenderMaterialValue::PxePropertyValue{ .ui32vec3 = value });
	}

	void PxeRenderMaterial::setProperty4ui(const std::string& name, const glm::u32vec4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::UINT4, PxeRenderMaterialValue::PxePropertyValue{ .ui32vec4 = value });
	}

	void PxeRenderMaterial::setPropertyM2f(const std::string& name, const glm::mat2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT2, PxeRenderMaterialValue::PxePropertyValue{ .fmat2 = value });
	}

	void PxeRenderMaterial::setPropertyM3f(const std::string& name, const glm::mat3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT3, PxeRenderMaterialValue::PxePropertyValue{ .fmat3 = value });
	}

	void PxeRenderMaterial::setPropertyM4f(const std::string& name, const glm::mat4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT4, PxeRenderMaterialValue::PxePropertyValue{ .fmat4 = value });
	}

	void PxeRenderMaterial::setPropertyM2x3f(const std::string& name, const glm::mat2x3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT2X3, PxeRenderMaterialValue::PxePropertyValue{ .fmat2x3 = value });
	}

	void PxeRenderMaterial::setPropertyM3x2f(const std::string& name, const glm::mat3x2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT3X2, PxeRenderMaterialValue::PxePropertyValue{ .fmat3x2 = value });
	}

	void PxeRenderMaterial::setPropertyM2x4f(const std::string& name, const glm::mat2x4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT2X4, PxeRenderMaterialValue::PxePropertyValue{ .fmat2x4 = value });
	}

	void PxeRenderMaterial::setPropertyM4x2f(const std::string& name, const glm::mat4x2& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT4X2, PxeRenderMaterialValue::PxePropertyValue{ .fmat4x2 = value });
	}

	void PxeRenderMaterial::setPropertyM3x4f(const std::string& name, const glm::mat3x4& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT3X4, PxeRenderMaterialValue::PxePropertyValue{ .fmat3x4 = value });
	}

	void PxeRenderMaterial::setPropertyM4x3f(const std::string& name, const glm::mat4x3& value)
	{
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::MAT4X3, PxeRenderMaterialValue::PxePropertyValue{ .fmat4x3 = value });
	}

	void PxeRenderMaterial::setProperty1fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty1f(name, values[0]);
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(float));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::FLOAT1, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty2fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty2f(name, glm::vec2(values[0], values[1]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(float) * 2);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::FLOAT2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty3fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty3f(name, glm::vec3(values[0], values[1], values[2]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(float) * 3);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::FLOAT3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty4fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty4f(name, glm::vec4(values[0], values[1], values[2], values[3]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(float) * 4);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::FLOAT4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty1iv(const std::string& name, const int32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty1i(name, values[0]);
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(int32_t));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::INT1, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty2iv(const std::string& name, const int32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty2i(name, glm::i32vec2(values[0], values[1]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(int32_t) * 2);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::INT2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty3iv(const std::string& name, const int32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty3i(name, glm::i32vec3(values[0], values[1], values[2]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(int32_t) * 3);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::INT3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty4iv(const std::string& name, const int32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty4i(name, glm::i32vec4(values[0], values[1], values[2], values[3]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(int32_t) * 4);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::INT4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty1uiv(const std::string& name, const uint32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty1ui(name, values[0]);
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(uint32_t));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::UINT1, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty2uiv(const std::string& name, const uint32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty2ui(name, glm::u32vec2(values[0], values[1]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(uint32_t) * 2);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::UINT2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty3uiv(const std::string& name, const uint32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty3ui(name, glm::u32vec3(values[0], values[1], values[2]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(uint32_t) * 3);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::UINT3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setProperty4uiv(const std::string& name, const uint32_t* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setProperty4ui(name, glm::u32vec4(values[0], values[1], values[2], values[3]));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(uint32_t) * 4);
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::UINT4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM2fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM2f(name, glm::make_mat2(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat2));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM3fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM3f(name, glm::make_mat3(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat3));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM4fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM4f(name, glm::make_mat4(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat4));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM2x3fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM2x3f(name, glm::make_mat2x3(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat2x3));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT2X3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM3x2fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM3x2f(name, glm::make_mat3x2(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat3x2));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT3X2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM2x4fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM2x4f(name, glm::make_mat2x4(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat2x4));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT2X4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM4x2fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM4x2f(name, glm::make_mat4x2(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat4x2));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT4X2, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM3x4fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM3x4f(name, glm::make_mat3x4(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat3x4));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT3X4, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setPropertyM4x3fv(const std::string& name, const float* values, uint32_t count)
	{
		if (!count) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue count must be greater then 0");
			return;
		}

		if (count == 1) {
			setPropertyM4x3f(name, glm::make_mat4x3(values));
			return;
		}

		PxeBuffer* buf = new(std::nothrow) PxeBuffer(count * sizeof(glm::mat4x3));
		if (buf == nullptr || !buf->getAllocated()) {
			PXE_ERROR("Failed to set PxeRenderMaterialValue, failed to allocate PxeBuffer");
			if (buf) buf->drop();
			return;
		}

		memcpy(buf->getBuffer(), values, buf->getSize());
		_materialProperties[name] = PxeRenderMaterialValue(count, PxeRenderMaterialValue::PxePropertyType::MAT4X3, PxeRenderMaterialValue::PxePropertyValue{ .buffer = buf });
	}

	void PxeRenderMaterial::setTexture(const std::string& name, PxeRenderTexture& texture, uint8_t textureSlot)
	{
		texture.grab();
		_materialProperties[name] = PxeRenderMaterialValue(1, PxeRenderMaterialValue::PxePropertyType::TEXTURE, PxeRenderMaterialValue::PxePropertyValue{ .texture = PxeRenderMaterialValue::PxePropertyValue::PxeTextureBinding { textureSlot, &texture } });
	}

	void PxeRenderMaterial::applyMaterial()
	{
		if (!_shader.getValid()) {
			PXE_WARN("Attempted to apply PxeRenderMaterial to invalid PxeShader");
			return;
		}

		if (_lastShaderCount != _shader.getAssetInitializationCount()) {
			PXE_INFO("Updated PxeRenderMaterial uniform locations");
			_lastShaderCount = _shader.getAssetInitializationCount();
			updatePropertyLocations();
		}

#ifdef PXE_DEBUG
		std::unordered_map<uint8_t, PxeRenderTexture*> boundTextures;
#endif // PXE_DEBUG


		for (auto it = _materialProperties.begin(); it != _materialProperties.end(); ++it) {
			const PxeRenderMaterialValue& materialProperty = it->second;
			if (materialProperty.ValueCount == 1) {
				switch (materialProperty.PropertyType)
				{
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT1:
					_shader.setUniform1i(materialProperty.UniformLocation, materialProperty.Value.i32);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT2:
					_shader.setUniform2i(materialProperty.UniformLocation, materialProperty.Value.i32vec2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT3:
					_shader.setUniform3i(materialProperty.UniformLocation, materialProperty.Value.i32vec3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT4:
					_shader.setUniform4i(materialProperty.UniformLocation, materialProperty.Value.i32vec4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT1:
					_shader.setUniform1ui(materialProperty.UniformLocation, materialProperty.Value.ui32);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT2:
					_shader.setUniform2ui(materialProperty.UniformLocation, materialProperty.Value.ui32vec2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT3:
					_shader.setUniform3ui(materialProperty.UniformLocation, materialProperty.Value.ui32vec3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT4:
					_shader.setUniform4ui(materialProperty.UniformLocation, materialProperty.Value.ui32vec4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT1:
					_shader.setUniform1f(materialProperty.UniformLocation, materialProperty.Value.f);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT2:
					_shader.setUniform2f(materialProperty.UniformLocation, materialProperty.Value.fvec2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT3:
					_shader.setUniform3f(materialProperty.UniformLocation, materialProperty.Value.fvec3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT4:
					_shader.setUniform4f(materialProperty.UniformLocation, materialProperty.Value.fvec4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2:
					_shader.setUniformM2f(materialProperty.UniformLocation, materialProperty.Value.fmat2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3:
					_shader.setUniformM3f(materialProperty.UniformLocation, materialProperty.Value.fmat3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4:
					_shader.setUniformM4f(materialProperty.UniformLocation, materialProperty.Value.fmat4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2X3:
					_shader.setUniformM2x3f(materialProperty.UniformLocation, materialProperty.Value.fmat2x3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3X2:
					_shader.setUniformM3x2f(materialProperty.UniformLocation, materialProperty.Value.fmat3x2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2X4:
					_shader.setUniformM2x4f(materialProperty.UniformLocation, materialProperty.Value.fmat2x4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4X2:
					_shader.setUniformM4x2f(materialProperty.UniformLocation, materialProperty.Value.fmat4x2);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3X4:
					_shader.setUniformM3x4f(materialProperty.UniformLocation, materialProperty.Value.fmat3x4);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4X3:
					_shader.setUniformM4x3f(materialProperty.UniformLocation, materialProperty.Value.fmat4x3);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::TEXTURE:
					_shader.setUniform1i(materialProperty.UniformLocation, materialProperty.Value.texture.Slot);
					materialProperty.Value.texture.renderTexture->setTextureSlot(materialProperty.Value.texture.Slot);
					materialProperty.Value.texture.renderTexture->bind();
#ifdef PXE_DEBUG
					{
						auto it = boundTextures.find(materialProperty.Value.texture.Slot);
						if (it != boundTextures.end() && it->second != materialProperty.Value.texture.renderTexture) {
							PXE_WARN("Bound multiple different PxeRenderTextures to the same gl texture slot, this is most likely an error");
						}
					}
#endif
					break;
				default:
					PXE_WARN("Attempted to apply NONE PxePropertyType to uniform " + it->first);
					break;
				}
			}
			else {
				switch (materialProperty.PropertyType)
				{
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT1:
					_shader.setUniform1iv(materialProperty.UniformLocation, (const int32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT2:
					_shader.setUniform2iv(materialProperty.UniformLocation, (const int32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT3:
					_shader.setUniform3iv(materialProperty.UniformLocation, (const int32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::INT4:
					_shader.setUniform4iv(materialProperty.UniformLocation, (const int32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT1:
					_shader.setUniform1uiv(materialProperty.UniformLocation, (const uint32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT2:
					_shader.setUniform2uiv(materialProperty.UniformLocation, (const uint32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT3:
					_shader.setUniform3uiv(materialProperty.UniformLocation, (const uint32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::UINT4:
					_shader.setUniform4uiv(materialProperty.UniformLocation, (const uint32_t*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT1:
					_shader.setUniform1fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT2:
					_shader.setUniform2fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT3:
					_shader.setUniform3fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::FLOAT4:
					_shader.setUniform4fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2:
					_shader.setUniformM2fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3:
					_shader.setUniformM3fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4:
					_shader.setUniformM4fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2X3:
					_shader.setUniformM2x3fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3X2:
					_shader.setUniformM3x2fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT2X4:
					_shader.setUniformM2x4fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4X2:
					_shader.setUniformM4x2fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT3X4:
					_shader.setUniformM3x4fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				case pxengine::PxeRenderMaterial::PxeRenderMaterialValue::PxePropertyType::MAT4X3:
					_shader.setUniformM4x3fv(materialProperty.UniformLocation, (const float*)materialProperty.Value.buffer->getBuffer(), materialProperty.ValueCount);
					break;
				default:
					PXE_WARN("Attempted to apply NONE array PxePropertyType to uniform " + it->first);
					break;
				}
			}
		}
	}

	PxeShader& PxeRenderMaterial::getShader() const
	{
		return _shader;
	}

	void PxeRenderMaterial::updatePropertyLocations()
	{
		for (auto it = _materialProperties.begin(); it != _materialProperties.end(); ++it) {
			it->second.UniformLocation = _shader.getUniformLocation(it->first);
		}
	}
}