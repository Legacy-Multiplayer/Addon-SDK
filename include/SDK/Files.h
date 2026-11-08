#pragma once

#include "Value.h"
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

namespace SDK
{
class CFiles
{
public:
    void Initialize(const FileSystemAPI* api)
    {
        m_api = api;
    }

    bool Exists(const std::string& path) const
    {
        return m_api && m_api->Exists(path.c_str());
    }

    std::string ReadText(const std::string& path) const
    {
        const auto handle = Open(path, "rb");

        struct Close
        {
            const FileSystemAPI* api;
            int handle;

            ~Close()
            {
                api->Close(handle);
            }
        } close{m_api, handle};

        const int size = m_api->Size(handle);

        if (size < 0 || size > 4 * 1024 * 1024)
        {
            throw std::length_error("Text file exceeds 4 MiB");
        }

        const char* text = m_api->Read(handle, static_cast<size_t>(size));

        if (!text || std::char_traits<char>::length(text) != static_cast<size_t>(size))
        {
            throw std::runtime_error("Cannot read text file: " + path);
        }

        return text;
    }

    void WriteText(const std::string& path, const std::string& text) const
    {
        if (text.size() > 4 * 1024 * 1024 || text.find('\0') != std::string::npos)
        {
            throw std::invalid_argument("Invalid text file data");
        }

        const auto handle = Open(path, "wb");

        struct Close
        {
            const FileSystemAPI* api;
            int handle;

            ~Close()
            {
                api->Close(handle);
            }
        } close{m_api, handle};

        if (m_api->Write(handle, text.c_str()) != static_cast<int>(text.size()) || !m_api->Flush(handle))
        {
            throw std::runtime_error("Cannot write text file: " + path);
        }
    }

private:
    int Open(const std::string& path, const char* mode) const
    {
        if (!m_api)
        {
            throw std::logic_error("File API unavailable");
        }

        const auto handle = m_api->Open(path.c_str(), mode);

        if (handle < 0)
        {
            throw std::runtime_error("Cannot open resource file: " + path);
        }

        return handle;
    }

    const FileSystemAPI* m_api = nullptr;
};

// Small INI-style configuration: key = "value". Load is transactional.

} // namespace SDK
