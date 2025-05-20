#ifndef INI_FILE_H_INCLUDED
#define INI_FILE_H_INCLUDED

#include <string>
#include <optional>
#include <map>

class IniFile
{
public:
    IniFile(const std::string& fileName);

    ~IniFile() = default;

    std::optional<std::string> getString(const std::string& key);
    std::optional<int> getInt(const std::string& key);
    std::optional<bool> getBool(const std::string& key);
    void setString(const std::string& key, const std::string& value);

    void save();

private:
    std::string m_fileName;
    std::map<std::string, std::string> m_data;
};


#endif