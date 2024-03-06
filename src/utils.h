#ifndef UTILS_H
#define UTILS_H

#ifdef WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <map>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>


namespace Utils{

    // vars
    enum class Operation { ADD, SUBTRACT, MULTIPLY, DIVIDE };
    enum class Method { PEARSON, SPEARMAN, KENDALL };

    // functions
    inline char getOperation(Operation op) {
        switch (op) {
            case Operation::ADD:
                return '+';
            case Operation::SUBTRACT:
                return '-';
            case Operation::MULTIPLY:
                return '*';
            case Operation::DIVIDE:
                return '/';
            default:
                throw std::invalid_argument("operation must be: +,-,*,/.");
        }
    }

    class FileFormatError : public std::exception {
    public:
        FileFormatError(const std::string& message) : message_(message) {}
        
        // 重写what()方法，返回异常描述信息
        const char* what() const noexcept override {
            return message_.c_str();
        }

    private:
        std::string message_;
    };

    /** whether a string starts with some substring
     * @param str whole string
     * @param pre substring
     * @return true if str starts with pre
     */
    inline bool startsWith(const std::string& str, const std::string& pre){
        if(str.length() < pre.length()){
            return false;
        }else{
            return std::equal(pre.begin(), pre.end(), str.begin());
        }
    }

    /** whether a string ends with some substring
     * @param str whole string
     * @param suf substring
     * @return true if str ends with suf
     */
    inline bool endsWith(const std::string& str, const std::string& suf){
        if(str.length() < suf.length()){
            return false;
        }else{
            return std::equal(suf.rbegin(), suf.rend(), str.rbegin());
        }
    }

    /**
     * @brief Get the delim char
     * 
     * @param filename 
     * @return char 
     */
    inline char getDelim(const std::string& filename) {
        if (endsWith(filename, "csv")) {
            return ',';
        } else if (endsWith(filename, "txt") || 
                endsWith(filename, "tsv")) {
            return '\t';
        } else {
            throw FileFormatError("File format error: must be csv,txt,tsv");
        }
    }
    
    /** get rid of the leading and ending white space characters of a string
     * @param str string to be stripped in both ends
     * @param pat string to be stripped from back
     * @return a string with white spaces stripped in both ends
     */
    inline std::string strip(const std::string& str, const std::string& pat = " \t\n\v\f\r"){
        std::string::size_type ipos = str.find_first_not_of(pat);
        if(ipos == std::string::npos){
            return "";
        }
        std::string::size_type epos = str.find_last_not_of(pat);
        if(epos == ipos){
            return str.substr(ipos);
        }else{
            return str.substr(ipos, epos - ipos + 1);
        }
    }

    /** get rid of the left leading white space characters of a string
     * @param str string to be stripped from front
     * @param pat string to be stripped from back
     * @return a string with left leading white spaces stripped
     */
    inline std::string lstrip(const std::string& str, const std::string& pat = " \t\n\v\f\r"){
        std::string::size_type pos = str.find_first_not_of(pat);
        if(pos == std::string::npos){
            return "";
        }
        return str.substr(pos);
    }

    /** get rid of the trailling white space characters of a string
     * @param str string to be stripped from back
     * @param pat string to be stripped from back
     * @return a string with right ending white spaces stripped
     */
    inline std::string rstrip(const std::string& str, const std::string& pat = " \t\n\v\f\r"){
        std::string::size_type pos = str.find_last_not_of(pat);
        if(pos == std::string::npos){
           return "";
        }
        return str.substr(0, pos + 1);
    } 

    /** remove prefix from a string
     * @param str string to be remove prefix
     * @param rms prefix to be removed
     */
    inline std::string lrmstr(const std::string& str, const std::string& rms){
        if(startsWith(str, rms)){
            return str.substr(rms.size());
        }
        return str;
    }

    /** remove suffix from a string
     * @param str string to be remove suffix
     * @param rms suffix to be removed
     */
    inline std::string rrmstr(const std::string& str, const std::string& rms){
        if(endsWith(str, rms)){
            return str.substr(0, str.size() - rms.size());
        }
        return str;
    }

    /** split a string by predefined seperator into a vector
     * @param str string
     * @param vec vector to store the split results
     * @param sep seperators, can contain a series of seperators
     */
    inline void split(const std::string& str, std::vector<std::string>& vec, std::string sep = " "){
        vec.clear(); // clear output vector
        if(str.empty()) return; // empty string, return directly
        std::string::size_type las, cur;
        las = cur = str.find_first_of(sep);
        if(las == std::string::npos){ // only one field without any sep
            vec.push_back(str);
            return;
        }
        if(las == 0) vec.push_back(""); // begin with sep, so there is a blank str before
        if(las != 0) vec.push_back(str.substr(0, las)); // begin with a non-sep field
        while(las != std::string::npos){
            if(las + 1 == std::string::npos){
                vec.push_back(""); // ends with sep, so there is a blank str after
                return;
            }
            cur = str.find_first_of(sep, las + 1);
            if(cur == std::string::npos){
                vec.push_back(str.substr(las + 1)); // last field
            }else{
                if(cur - las == 1){
                    vec.push_back(""); // consecutive sep
                }else{
                    vec.push_back(str.substr(las + 1, cur - las - 1));
                }
            }
            las = cur;
        }
    }

    /** join a list of strings by an seperator
     * @param vec vector to store the split results
     * @param ret joined string
     * @param sep seperator used to join strings
     */
    inline void join(const std::vector<std::string>& vec, std::string& ret, const std::string& sep = " "){
        if(vec.empty()) return;
        if(vec.size() == 1){
            ret = vec[0];
            return;
        }
        for(uint32_t i = 0; i < vec.size() - 1; ++i){
            ret.append(vec[i] + sep);
        }
        ret.append(vec[vec.size() - 1]);
    }
    
    /** join a list of strings by an seperator
     * @param vec vector to store the split results
     * @param sep seperator used to join strings
     * @return joined string
     */
    template <typename T>
    inline std::string join(const std::vector<T>& vec, const std::string& sep = " "){
        if(vec.empty()) return "";
        std::stringstream ss;
        ss << vec[0];
        if(vec.size() == 1){
            return ss.str();
        }
        for(uint32_t i = 1; i < vec.size(); ++i){
            ss << sep << vec[i];
        }
        return ss.str();
    }

    /** replace a substr apearing in a string with another string
     * @param str string 
     * @param pat substr of string to be replaced
     * @param des string to be used to replaced with pat
     * @return a string with each pat replaced by des
     */
    inline std::string replace(const std::string& str, const std::string& pat, const std::string& des){
        std::string ret;
        std::string::size_type las = 0, cur = 0;
        while((cur = str.find(pat, cur)) != std::string::npos){
            ret.append(str.substr(las, cur - las));
            ret.append(des);
            cur += pat.length();
            las = cur;
        }
        if(las != std::string::npos){
            ret.append(str.substr(las));
        }
        return ret;
    }

    /** get a reverse sequence of str
     * @param str input string sequence
     * @return reverse sequence of str
     */
    inline std::string reverse(const std::string& str){
        std::string ret = str;
        std::reverse(ret.begin(), ret.end());
        return ret;
    }

    /** get the basename of a path string
     * @param path name
     * @param ext to be removed
     * @return basename of path
     */
    inline std::string basename(const std::string& path, const std::string ext = ""){
        std::string fpath = Utils::replace(path, "~", std::string(std::getenv("HOME")) + "/");
        if(fpath.find_first_of("\t\n\v\f\r") != std::string::npos){
            return "";
        }
        std::string::size_type pos1 = fpath.find_last_of("/\\");
        if(pos1 == std::string::npos){
            return rrmstr(fpath, ext);
        }
        std::string::size_type pos2 = fpath.find_last_not_of("/\\");
        if(pos2 == fpath.size() - 1){
            return rrmstr(fpath.substr(pos1 + 1, pos2 - pos1), ext);
        }
        std::string::size_type pos3 = fpath.find_last_of("/\\", pos2);
        if(pos3 == std::string::npos){
            return rrmstr(fpath.substr(0, pos2 + 1), ext);
        }
        return rrmstr(fpath.substr(pos3 + 1, pos2 - pos3), ext);
    }

    /** get the dirname of a path string
     * @param path name
     * @return dirname of path
     */
    inline std::string dirname(const std::string& path){
        std::string fpath = Utils::replace(path, "~", std::string(std::getenv("HOME")) + "/");
        std::string::size_type pos = fpath.find_last_of("/\\");
        if(pos == std::string::npos){
#ifdef _WIN32
            return ".\\";
#else
            return "./";
#endif
        }
        if(pos == fpath.size() - 1){
            std::string::size_type pos1 = fpath.find_last_not_of("/\\");
            if(pos1 == std::string::npos){
#ifdef _WIN32
                return "\\";
#else
                return "/";
#endif
            }
            std::string::size_type pos2 = fpath.find_last_of("/\\", pos1);
            if(pos2 == std::string::npos){
#ifdef _WIN32
                return ".\\";
#else
                return "./";
#endif
            }else{
                return fpath.substr(0, pos2 + 1);
            }
        }else{
            return fpath.substr(0, pos + 1);
        }
    }

    /** get absolute path of a path
     * @param path string of path
     * @return absolute path string
     */
    inline std::string abspath(const std::string& path){
        std::string newPath = Utils::strip(path);
        if(newPath.length() == 0){
            return "";
        }
        char cpath[FILENAME_MAX];
#ifdef _WIN32
        _realpath(newPath.c_str(), cpath);
#else
        realpath(newPath.c_str(), cpath);
#endif
        return cpath;
    }

    /** get current working directory
     * @return current working directory
     */
    inline std::string cwd(void){
        char cpath[FILENAME_MAX];
#ifdef _WIN32
        _getcwd(cpath, FILENAME_MAX);
#else
        getcwd(cpath, FILENAME_MAX);
#endif
        return cpath;
    }

    /** join dirname and basename into a path
     * @param dirname dirname of the path
     * @param basename basename of the path
     * @return full path
     */
    inline std::string joinpath(const std::string& dirname, const std::string& basename){
#ifdef _WIN32
        return dirname + "\\" + basename;
#else
        return dirname + "/" + basename;
#endif
    }

    /** check a string is a regular file or not
     * @param path string of a file/directory
     * @return true if path is an existing regular file
     */
    inline bool isfile(const std::string& path){
#ifdef _WIN32
        struct _stat info;
        if(_stat(path.c_str(), &info) != 0){
            return false;
        }
        return (info.st_mode & _S_IFREG);
#else
        struct stat info;
        if(stat(path.c_str(), &info) !=0){
            return false;
        }
        return (info.st_mode & S_IFREG);
#endif
    }

    /** check a string is a directory or not
     * @param path string of a file/directory
     * @return true if path is an existing path
     */
    inline bool isdir(const std::string& path){
#ifdef _WIN32
        struct _stat info;
        if(_stat(path.c_str(), &info) != 0){
            return false;
        }
        return (info.st_mode & _S_IFDIR);
#else
        struct stat info;
        if(stat(path.c_str(), &info) != 0){
            return false;
        }
        return (info.st_mode & S_IFDIR);
#endif
    }
    
    /** check a file/directory exists or not
     * @param path string of a file/directory
     * @return true if path exists
     */
    inline bool exists(const std::string& path){
#ifdef _WIN32
        struct _stat s;
        return (_stat(path.c_str(), &s) == 0);
#else
        struct stat s;
        return (stat(path.c_str(), &s) == 0);
#endif
    }

    /** exit and print string to std::cerr
     * @param msg string to print to std::cerr
     * @param f log file fp
     */
    inline void errorExit(const std::string& msg, FILE* f = stderr){
        time_t tt = time(NULL);
        tm* t = std::localtime(&tt);
        char date[60] = {0};
        std::snprintf(date, sizeof(date), "[%d-%02d-%02d %02d:%02d:%02d] ",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec);
        fprintf(f, "%s%s\n", date, msg.c_str());
        fflush(f);
        exit(-1);
    }
    
    /** check a file existence status, if not exists, exit 
     * @param path string of file/directory
     */
    inline void validFile(const std::string& path){
        if(Utils::isdir(path)){
            Utils::errorExit("this is not a file path!");
        }
        if(!Utils::isfile(path)){
            Utils::errorExit("file does not exist");
        }
    }

    /** make directories recursively
     * @param path path of directory to be created
     * @return true if make directories successfully
     */
    inline bool makedir(const std::string& path){
        std::string fpath = Utils::replace(path, "~", std::string(std::getenv("HOME")) + "/");
#ifdef _WIN32
        int ret = _mkdir(fpath.c_str());
#else
        mode_t mode = 0755;
        int ret = mkdir(fpath.c_str(), mode);
#endif
        if(ret == 0){
            return true;
        }
        switch(errno){
            case ENOENT:
                {
                    if(!Utils::makedir(Utils::dirname(fpath))){
                        return false;
                    }
                }
#ifdef _WIN32
                return 0 == _mkdir(fpath.c_str());
#else
                return 0 == mkdir(fpath.c_str(), mode);
#endif
            case EEXIST:
                return Utils::isdir(fpath);
            default:
                return false;
        }
    }

    /** touch a file
     * @param path file path to touch
     */
    inline void touch(const std::string& path){
        if(Utils::exists(path)) return;
        std::string dirpath = Utils::dirname(path);
        if(!Utils::exists(dirpath)) Utils::makedir(dirpath);
        std::ofstream fw(path);
        fw.close();
    }

    /** make a string each character uppercased
     * @param str string to be uppercased
     */
    inline void str2upper(std::string& str){
        std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::toupper);
    }

    /** make a string each character lowercased
     * @param str string to be lowercased
     */
    inline void str2lower(std::string& str){
        std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::tolower);
    }

    /** get current time
     * @return year-mm-dd hh-mm-ss of current time
     */
    inline std::string currentTime(void){
        time_t tt = time(NULL);
        tm* t = std::localtime(&tt);
        char date[60] = {0};
        std::snprintf(date, sizeof(date), "%d-%02d-%02d %02d:%02d:%02d",
                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                t->tm_hour, t->tm_min, t->tm_sec);
        return date;
    }
}

#endif