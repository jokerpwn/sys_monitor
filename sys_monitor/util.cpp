#include "util.h"

#include <qdiriterator.h>
map<char,const string> util::state_map={
{'R', "Running"},
{'S', "Sleeping"},
{'D', "Waiting"},
{'Z', "Zombie"},
{'T', "Stopped"},
{'t', "Tracing stop"},
{'W', "Paging"},
{'X', "Dead"},
{'x', "Dead"},
{'K', "Wakekill"},
{'P', "Parked"}
};
util::util()
{
}
QString util::getProcessCmdline(pid_t pid)
{
    std::string temp;
    try {
        std::fstream fs;
        fs.open("/proc/"+std::to_string((long)pid)+"/cmdline", std::fstream::in);
        std::getline(fs,temp);
        fs.close();
    } catch(std::ifstream::failure e) {
        return "FAILED TO READ PROC";
    }

    // change \0 to ' '
    std::replace(temp.begin(),temp.end(),'\0',' ');

    if (temp.size()<1) {
        return "";
    }
    return QString::fromStdString(temp);;
}
/**
     * @brief getProcessIconFromName Get the icon for a process given its name
     * @param procname The name of the process
     * @return The process' icon or the default executable icon if none was found
     */
QIcon util::getProcessIconFromName(QString procName, std::unordered_map<QString, QIcon> &processIconMapCache)
{
    // check we havent already got the icon in the cache
    auto pos = processIconMapCache.find(procName);
    if (pos != processIconMapCache.end()) {
        return pos->second;
    }

    // apply some corrections to the process name
    // ie, sh should look for terminal icons, not anything containing sh
    static std::map<QString, QString> procNameCorrections({
        {"sh","terminal"}, {"bash","terminal"}, {"dconf-service", "dconf"}, {"gconfd-2", "dconf"}, {"deja-dup-monitor", "deja-dup"}
    });
    auto procPos = procNameCorrections.find(procName);
    if (procPos != procNameCorrections.end()) {
        procName = procPos->second;
    }

    // search /usr/share/applications for the desktop file that corresponds to the proc and get its icon
    QDirIterator dir("/usr/share/applications", QDirIterator::Subdirectories);
    QString desktopFile;
    QIcon defaultExecutableIcon = QIcon::fromTheme("application-x-executable");
    while(dir.hasNext()) {
        if (dir.fileInfo().suffix() == "desktop") {
            if (dir.fileName().toLower().contains(procName.toLower())) {
                desktopFile = dir.filePath();
                break;
            }
        }
        dir.next();
    }

    if (desktopFile.size() == 0) {
        return defaultExecutableIcon;
    }

    QIcon icon = defaultExecutableIcon;
    QString iconName;
    QFile in(desktopFile);
    in.open(QIODevice::ReadOnly);
    while(!in.atEnd()) {
        iconName = in.readLine().trimmed();
        if (iconName.startsWith("Icon=")) {
            iconName.remove(0,5); // remove the first 5 chars
        } else {
            continue;
        }

        if (iconName.contains("/")) {
            // this is probably a path to the file, use that instead of the theme icon name
            icon = QIcon(iconName);
        } else {
            icon = QIcon::fromTheme(iconName,defaultExecutableIcon);
            break;
        }
    }
    in.close();

    processIconMapCache[procName] = icon;
    return icon;
}


string util::file_user(const struct stat* file_message)
{
    // 根据用户id获取用户名
    struct passwd* pwd;
    pwd = getpwuid(file_message->st_uid);
    return string(pwd->pw_name);
#if 0
    struct passwd
    {
        char * pw_name; /* Username, POSIX.1 */
        char * pw_passwd; /* Password */
        __uid_t pw_uid; /* User ID, POSIX.1 */
        __gid_t pw_gid; /* Group ID, POSIX.1 */
        char * pw_gecos; /* Real Name or Comment field */
        char * pw_dir; /* Home directory, POSIX.1 */
        char * pw_shell; /* Shell Program, POSIX.1 */
    };
#endif//0
}
