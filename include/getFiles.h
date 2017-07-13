#include <vector>
#include <string>
#ifdef _WIN64
#include <Windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace getFiles
{
    std::vector<std::string> h5InFolder(const std::string& folder)
    {
        std::vector<std::string> names;
#ifdef _WIN64
        std::string search_path = folder + "/*.h5";
        WIN32_FIND_DATA fd;
        HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
        if(hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) files in current folder
                // , delete '!' read other 2 default folder . and ..
                if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    names.push_back(fd.cFileName);
                }
            }while(::FindNextFile(hFind, &fd));
            ::FindClose(hFind);
        }
        return names;
#else
        DIR *dir;
        class dirent *ent;
        class stat st;

        dir = opendir(folder.c_str());
        while ((ent = readdir(dir)) != NULL) {
            const std::string file_name = ent->d_name;
            const std::string full_file_name = folder + "/" + file_name;

            if (file_name.back() != '5')
                continue;

            if (stat(full_file_name.c_str(), &st) == -1)
                continue;

            const bool is_directory = (st.st_mode & S_IFDIR) != 0;

            if (is_directory)
                continue;

            names.push_back(full_file_name);
        }
        closedir(dir);
    }
#endif
}
}
