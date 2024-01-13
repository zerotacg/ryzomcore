#ifndef LIBGLTF_JSON_WRITER_H
#define LIBGLTF_JSON_WRITER_H

#include <iostream>
#include <optional>
#include <vector>

#include <nel/misc/types_nl.h>

namespace gltf {

struct JsonWriter
{
	FILE *file;
	std::vector<std::string> propertyPrefix;

	template <class T, class Allocator>
	void write(const std::vector<T, Allocator> &container)
	{
		fprintf(file, "[");
		for (auto it = container.begin(); it != container.end(); ++it)
		{
			if (it != container.begin())
			{
				fprintf(file, ", ");
			}
			write(*it);
		}
		fprintf(file, "]");
	}

	template <class T>
	void write(const T &object)
	{
		fprintf(file, "{ ");
		propertyPrefix.push_back("");
		object.write(*this);
		propertyPrefix.pop_back();
		fprintf(file, " }");
	}

	void write(const std::string &value)
	{
		fprintf(file, R"("%s")", value.c_str());
	}

	void write(const uint32 &value)
	{
		fprintf(file, "%i", value);
	}
	void write(const sint32 &value)
	{
		fprintf(file, "%i", value);
	}

	void write(const size_t &value)
	{
		fprintf(file, "%lu", value);
	}

	void write(const float &value)
	{
		fprintf(file, "%f", value);
	}

	template <class T>
	void writeProperty(const std::string &key, const T &value)
	{
		fprintf(file, "%s", propertyPrefix.back().c_str());
		write(key);
		fprintf(file, ": ");
		write(value);
		propertyPrefix.back() = ", ";
	}

	template <class T>
	void writeProperty(const std::string &key, const std::vector<T> &value)
	{
		if(!value.empty())
		{
			writeProperty(key, value);
		}
	}

	template <class T>
	void writeProperty(const std::string &key, const std::optional<T> &value)
	{
		if(value.has_value())
		{
			writeProperty(key, value.value());
		}
	}
};

}

#endif // LIBGLTF_JSON_WRITER_H
