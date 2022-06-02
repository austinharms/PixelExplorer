#include "Material.h"
#include "Logger.h"

namespace pixelexplorer::rendering {
	Material::Material()
	{
	}

	Material::~Material()
	{
		for (auto it = _properties.begin(); it != _properties.end(); ++it)
			delete it->second;
	}

	void Material::applyMaterial(Shader* shader)
	{
		for (auto it = _properties.begin(); it != _properties.end(); ++it)
			it->second->applyProperty(shader, it->first);
	}

	void Material::removeProperty(const std::string& propertyName)
	{
		auto it = _properties.find(propertyName);
		if (it == _properties.end()) {
			Logger::warn("Attempted to remove non existent material property " + propertyName);
		}
		else {
			_properties.erase(it);
		}
	}

	void Material::setProperty(const std::string& propertyName, float value)
	{
		setProperty(propertyName, new floatProperty(value));
	}

	void Material::setProperty(const std::string& propertyName, int32_t value)
	{
		setProperty(propertyName, new int32Property(value));
	}

	void Material::setProperty(const std::string& propertyName, glm::vec2 value)
	{
		setProperty(propertyName, new vec2Property(value));
	}

	void Material::setProperty(const std::string& propertyName, glm::vec3 value)
	{
		setProperty(propertyName, new vec3Property(value));
	}

	void Material::setProperty(const std::string& propertyName, glm::vec4 value)
	{
		setProperty(propertyName, new vec4Property(value));
	}

	void Material::setProperty(const std::string& propertyName, glm::mat4 value)
	{
		setProperty(propertyName, new mat4Property(value));
	}

	void Material::setProperty(const std::string& propertyName, MaterialProperty* prop)
	{
		auto it = _properties.find(propertyName);
		if (it == _properties.end()) {
			_properties[propertyName] = prop;
		}
		else {
			delete it->second;
			it->second = prop;
		}
	}
}