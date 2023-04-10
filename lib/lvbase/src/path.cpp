#include "path.h"
#include "live/visuallog.h"

#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#  if(__GNUC__ > 7)
#    include <filesystem>
     namespace fs = std::filesystem;
#  else
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  endif
#else
#  include <filesystem>
   namespace fs = std::filesystem;
#endif

namespace lv{

// Helpers
// ----------------------------------------------------------------------------

namespace{

template <typename TP> std::chrono::system_clock::time_point toTimePoint(TP tp){
    return std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        tp - TP::clock::now() + std::chrono::system_clock::now()
    );
}

} // namespace


// class Path
// ----------------------------------------------------------------------------

std::string Path::temporaryDirectory(){
    return fs::temp_directory_path().string();
}

bool Path::exists(const std::string &s){
    return fs::exists(s);
}

bool Path::createDirectory(const std::string &p){
    return fs::create_directory(p);
}

bool Path::createDirectories(const std::string &p){
    return fs::create_directories(p);
}

bool Path::isDir(const std::string &p){
    return fs::is_directory(p);
}

bool Path::remove(const std::string &p){
    return fs::remove_all(p);
}

void Path::rename(const std::string &from, const std::string &to){
    fs::rename(from, to);
}

/**
 * \brief Copies file specified in \p from, to file specified by \p to.
 */
bool Path::copyFile(const std::string &from, const std::string &to, int options){
    if ( !Path::isFile(from) )
        THROW_EXCEPTION(lv::Exception, Utf8("Path is not a file: %").format(from), lv::Exception::toCode("~File"));

    fs::copy_options sendOpts = fs::copy_options::recursive;
    if ( options & Path::OverwriteExisting )
        sendOpts |= fs::copy_options::overwrite_existing;
    if ( options & Path::SkipExisting )
        sendOpts |= fs::copy_options::skip_existing;
    if ( options & Path::UpdateExisting )
        sendOpts |= fs::copy_options::update_existing;

    return fs::copy_file(from, to, sendOpts);
}

/**
 * \brief Copies directory from, to directory to, recursively going through all files
 *
 * If \p to doesn't exist, it's created, and its contents will be the same as \p from
 * If it does exit, the behaviour is defined by the \p optiosn argument.
 */
void Path::copyRecursive(const std::string &from, const std::string &to, int options){
    if ( Path::isFile(from) )
        THROW_EXCEPTION(lv::Exception, Utf8("Path is a file: %. Use Path::copyFile instead.").format(from), lv::Exception::toCode("File"));

    fs::copy_options sendOpts = fs::copy_options::recursive;
    if ( options & Path::OverwriteExisting )
        sendOpts |= fs::copy_options::overwrite_existing;
    if ( options & Path::SkipExisting )
        sendOpts |= fs::copy_options::skip_existing;
    if ( options & Path::UpdateExisting )
        sendOpts |= fs::copy_options::update_existing;

    fs::copy(from, to, sendOpts);
}

std::string Path::join(const std::string &p1, const std::string &p2){
    if ( p2.empty() )
        return p1;
    return (fs::path(p1) / fs::path(p2)).string();
}

std::vector<std::string> Path::split(const std::string p){
    std::vector<std::string> result;
    fs::path path(p);
    for( auto& part : path )
        result.push_back(part.filename().string());
    return result;
}

/**
 * \brief Returns the file name including the extension (i.e. filename.txt)
 */
std::string Path::name(const std::string &p){
    return fs::path(p).filename().string();
}

std::string Path::parent(const std::string &p){
    return fs::path(p).parent_path().string();
}

std::string Path::absolutePath(const std::string &p){
    return fs::absolute(p).string();
}

std::string Path::resolve(const std::string &p){
    try{
        std::string res = fs::canonical(p).string();
        return res;
    } catch ( std::exception& e ){
        THROW_EXCEPTION(lv::Exception, e.what(), Exception::toCode("~Path"));
    }
    return "";
}

std::string Path::rootPath(const std::string &p){
    return fs::path(p).root_path().string();
}

bool Path::isRelative(const std::string &p){
    return fs::path(p).is_relative();
}

bool Path::isAbsolute(const std::string &p){
    return fs::path(p).is_absolute();
}

bool Path::isFile(const std::string &p){
    return fs::is_regular_file(p);
}

/**
 * \brief Returns the filename without the extension (i.e. 'file.txt' returns 'file')
 */
std::string Path::baseName(const std::string &p){
    std::string filename = Path::name(p);
    auto f = filename.find('.');
    if ( f == std::string::npos )
        return filename;
    return filename.substr(0, f);
}

std::string Path::extension(const std::string &p){
    return fs::path(p).extension().string();
}

std::string Path::suffix(const std::string &p){
    std::string filename = Path::name(p);
    auto f = filename.rfind('.');
    if ( f == std::string::npos )
        return "";
    return filename.substr(f + 1);
}

std::string Path::completeSuffix(const std::string &p){
    std::string filename = Path::name(p);
    auto f = filename.find('.');
    if ( f == std::string::npos )
        return "";
    return filename.substr(f + 1);
}

bool Path::hasExtensions(const std::string &p, const std::vector<std::string> &ext){
    Utf8 utfp = p;
    for ( const std::string& e : ext ){
        if ( utfp.endsWith(e) ){
            return true;
        }
    }
    return false;
}

int Path::permissions(const std::string &p){
    auto perm = fs::status(p).permissions();

    int result = 0;
    if ((perm & fs::perms::group_read) != fs::perms::none) result = result | 256;     // 100 000 000
    if ((perm & fs::perms::owner_write) != fs::perms::none) result = result | 128;    // 010 000 000
    if ((perm & fs::perms::owner_exec) != fs::perms::none) result = result | 64;       // 001 000 000

    if ((perm & fs::perms::group_read) != fs::perms::none) result = result | 32;      // 000 100 000
    if ((perm & fs::perms::group_write) != fs::perms::none) result = result | 16;     // 000 010 000
    if ((perm & fs::perms::group_exec) != fs::perms::none) result = result | 8;        // 000 001 000

    if ((perm & fs::perms::others_read) != fs::perms::none) result = result | 4;       // 000 000 100
    if ((perm & fs::perms::others_write) != fs::perms::none) result = result | 2;      // 000 000 010
    if ((perm & fs::perms::others_exec) != fs::perms::none) result = result | 1;        // 000 000 001

    return result;
}

void Path::setPermissions(const std::string &p, int value){

    fs::perms perm = fs::perms::none;

    if (value & 256)    perm = perm | fs::perms::owner_read;
    if (value & 128)    perm = perm | fs::perms::owner_write;
    if (value & 64)     perm = perm | fs::perms::owner_exec;

    if (value & 32)     perm = perm | fs::perms::group_read;
    if (value & 16)     perm = perm | fs::perms::group_write;
    if (value & 8)      perm = perm | fs::perms::group_exec;

    if (value & 4)      perm = perm | fs::perms::others_read;
    if (value & 2)      perm = perm | fs::perms::others_write;
    if (value & 1)      perm = perm | fs::perms::others_exec;

    fs::permissions(p, perm);
}

void Path::createSymlink(const std::string &link, const std::string &path){
    if ( Path::isDir(path) ){
        return fs::create_directory_symlink(path, link);
    } else {
        return fs::create_symlink(path, link);
    }
}

bool Path::isSymlink(const std::string &p){
    return fs::is_symlink(p);
}

std::string Path::followSymlink(const std::string &p){
    return fs::read_symlink(p).string();
}

std::string Path::relativePath(const std::string &referencePath, const std::string &path){
    std::string reference = Path::isFile(referencePath) ? Path::parent(referencePath) : referencePath;

    auto referenceSegm = Path::split(reference);
    auto pathSegm = Path::split(path);

    std::vector<std::string> relativePathSegments;
    size_t i = 0;
    while ( i < referenceSegm.size() && i < pathSegm.size() ){
        if ( referenceSegm[i] != pathSegm[i] )
            break;
        ++i;
    }

    if ( i == 0 ){
        return path;
    } else {
        for ( size_t j = i; j < referenceSegm.size(); ++j ){
            if ( !referenceSegm[j].empty() )
                relativePathSegments.push_back("..");
        }
        for ( size_t j = i; j < pathSegm.size(); ++j ){
            if ( !pathSegm[j].empty() )
                relativePathSegments.push_back(pathSegm[j]);
        }
    }

    std::string result;
    for ( const std::string& s : relativePathSegments )
        result += result.empty() ? s : ("/" + s);
    return result;
}

DateTime Path::lastModified(const std::string &path){
    std::chrono::system_clock::time_point t = toTimePoint(fs::last_write_time(path));
    auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(t).time_since_epoch().count();
    return DateTime::createFromMs(ms);
}

Path::Path(){
}

}// namespace
